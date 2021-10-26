import dlib
import cv2
import numpy as np
import math
import matplotlib.pyplot as plt
from PIL import Image
from PIL import ImageFilter


predictor_model ='shape_predictor_81_face_landmarks.dat'
detector = dlib.get_frontal_face_detector()
predictor = dlib.shape_predictor(predictor_model)
 
def landmark_dec_dlib_fun(img):
    img_gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    landmarks_all = []
    rects = detector(img_gray, 0) # 人脸数，可能多个人脸
    for i in range(len(rects)):
        landmarks = np.matrix([[p.x,p.y] for p in predictor(img_gray,rects[i]).parts()])
        landmarks_all.append(landmarks)
    return landmarks_all
 
#双线性插值法
def BilinearInsert(img, ux, uy):
    x1 = int(ux)
    x2 = x1+1
    y1 = int(uy)
    y2 = y1+1
 
    part1 = img[y1, x1].astype(np.float) * (float(x2) - ux) * (float(y2) - uy)
    part2 = img[y1, x2].astype(np.float) * (ux - float(x1)) * (float(y2) - uy)
    part3 = img[y2, x1].astype(np.float) * (float(x2) - ux) * (uy - float(y1))
    part4 = img[y2, x2].astype(np.float) * (ux - float(x1)) * (uy - float(y1))

    insertValue = part1 + part2 + part3 + part4

    return insertValue.astype(np.int8)
 
def BigEyesFunction(img, alpha = 0.7):
 
    landmarks_all = landmark_dec_dlib_fun(img)

    for landmarks in landmarks_all:
        left_x = math.ceil(np.mean([landmarks[37][0, 0], landmarks[38][0, 0], landmarks[40][0, 0], landmarks[41][0, 0]]))
        left_y = math.ceil(np.mean([landmarks[37][0, 1], landmarks[38][0, 1], landmarks[40][0, 1], landmarks[41][0, 1]]))
        left_center = np.array([left_x, left_y]).reshape(1, 2)

        right_x = math.ceil(np.mean([landmarks[43][0, 0], landmarks[44][0, 0], landmarks[46][0, 0], landmarks[47][0, 0]]))
        right_y = math.ceil(np.mean([landmarks[43][0, 1], landmarks[44][0, 1], landmarks[46][0, 1], landmarks[47][0, 1]]))
        right_center = np.array([right_x, right_y]).reshape(1, 2)
        
        ref = landmarks[27]
 
        left_rmax  = math.sqrt((left_center - ref).dot((left_center - ref).T)) * alpha
        right_rmax = math.sqrt((right_center - ref).dot((right_center - ref).T)) * alpha
 
        lbigger  = EyeTransform(img    ,  left_center,  left_rmax)
        lrbigger = EyeTransform(lbigger, right_center, right_rmax)
    return lrbigger

def EyeTransform(img, center, rmax):
    outimg = img.copy()
    for x in range(img.shape[1]):
        for y in range(img.shape[0]):
            point = np.array([x, y])
            r = math.sqrt((point - center).dot((point - center).T))
            if(r < rmax):
                rate = 1 - math.pow(r / rmax - 1, 2) * 0.5
                # 根据公式计算新的X Y 座标
                X = center[0, 0] + rate * (x - center[0, 0])
                Y = center[0, 1] + rate * (y - center[0, 1])
                # 插值
                if (int(X) < img.shape[1] - 1) and (int(Y) < img.shape[0] - 1):
                    outimg[y, x] = BilinearInsert(img, X, Y)
 
    return outimg
 
def LightEyesFunction(img, alpha = 0.1):

    img_gray = cv2.cvtColor(img, cv2.COLOR_RGB2GRAY)
    dst = cv2.Laplacian(img_gray, cv2.CV_16S, ksize = 3)
    Laplacian = cv2.convertScaleAbs(dst, alpha = alpha)

    landmarks_all = landmark_dec_dlib_fun(img)

    # get contours
    left_contour = []
    right_contour = []
    for landmarks in landmarks_all:
        left_contour.append([landmarks[36], landmarks[37], landmarks[38], landmarks[39], landmarks[40], landmarks[41]])
        right_contour.append([landmarks[42], landmarks[43], landmarks[44], landmarks[45], landmarks[46], landmarks[47]])
    left_contour = np.array(left_contour).reshape(-1, 2)
    right_contour = np.array(right_contour).reshape(-1, 2)

    # get mask
    mask = np.zeros(img.shape[0 : 2], dtype="uint8")
    cv2.fillPoly(mask, np.int32([left_contour, right_contour]), 1)
    enhance = Laplacian * mask # all enhance < 0

    # change in YUV
    imgout = cv2.cvtColor(img, cv2.COLOR_BGR2YUV)
    
    imgout[:, :, 0] = np.where(imgout[:, :, 0] - enhance >= 245, imgout[:, :, 0], imgout[:, :, 0] - enhance)
    imgout = cv2.cvtColor(imgout, cv2.COLOR_YUV2BGR)
    return imgout

def FaceTransform(img, center, ref, rmax):
    imgout = img.copy()
    dist = math.sqrt((center - ref).dot((center - ref).T))
    for x in range(img.shape[1]):
        for y in range(img.shape[0]):
            point = np.array([x, y])
            r = math.sqrt((point - center).dot((point - center).T))
            if r < rmax:
                rate = math.pow((math.pow(rmax, 2) - math.pow(r, 2)) / (math.pow(rmax, 2) - math.pow(r, 2) + math.pow(dist, 2)), 2)

                # 根据公式计算新的X Y 座标
                X = x - rate * (ref[0, 0] - center[0, 0])
                Y = y - rate * (ref[0, 0] - center[0, 1])
                # 插值
                if (int(X) < img.shape[1] - 1) and (int(Y) < img.shape[0] - 1):
                    imgout[y, x] = BilinearInsert(img, X, Y)
    return imgout


def ShrinkFaceFunction(img, alpha = 0.8):

    landmarks_all = landmark_dec_dlib_fun(img)
    for landmarks in landmarks_all:
        left_rmax  = math.sqrt((landmarks[2] -  landmarks[4]).dot((landmarks[2] -  landmarks[4]).T)) * alpha
        right_rmax = math.sqrt((landmarks[12] -  landmarks[14]).dot((landmarks[12] -  landmarks[14]).T)) * alpha

        ref = landmarks[30]

        shrinkl  = FaceTransform(img    , landmarks[3], ref, left_rmax)
        shrinklr = FaceTransform(shrinkl, landmarks[13], ref, right_rmax)

    return shrinklr, landmarks_all

color_transform_list = [
	1, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 31, 33, 35, 37, 39,
	41, 43, 44, 46, 48, 50, 52, 53, 55, 57, 59, 60, 62, 64, 66, 67, 69, 71, 73, 74,
	76, 78, 79, 81, 83, 84, 86, 87, 89, 91, 92, 94, 95, 97, 99, 100, 102, 103, 105,
	106, 108, 109, 111, 112, 114, 115, 117, 118, 120, 121, 123, 124, 126, 127, 128,
	130, 131, 133, 134, 135, 137, 138, 139, 141, 142, 143, 145, 146, 147, 149, 150,
	151, 153, 154, 155, 156, 158, 159, 160, 161, 162, 164, 165, 166, 167, 168, 170,
	171, 172, 173, 174, 175, 176, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187,
	188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203,
	204, 205, 205, 206, 207, 208, 209, 210, 211, 211, 212, 213, 214, 215, 215, 216,
	217, 218, 219, 219, 220, 221, 222, 222, 223, 224, 224, 225, 226, 226, 227, 228,
	228, 229, 230, 230, 231, 232, 232, 233, 233, 234, 235, 235, 236, 236, 237, 237,
	238, 238, 239, 239, 240, 240, 241, 241, 242, 242, 243, 243, 244, 244, 244, 245,
	245, 246, 246, 246, 247, 247, 248, 248, 248, 249, 249, 249, 250, 250, 250, 250,
	251, 251, 251, 251, 252, 252, 252, 252, 253, 253, 253, 253, 253, 254, 254, 254,
	254, 254, 254, 254, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 256]

def SkinColormask(img): # 7 < H < 20 28 < S < 256 50 < V < 256
    hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    h, s, v = cv2.split(hsv)
    mask = np.zeros((img.shape[0], img.shape[1]))
    mask = np.where((h > 7) & (h < 20) & (s > 28) & (s < 256) & (v > 50) & (v < 256), 1, 0)
    return mask    

def SkinColormask1(img):
    skinCrCbHist = np.zeros((img.shape[0],img.shape[1]), dtype= np.uint8 )
    cv2.ellipse(skinCrCbHist, (113, 155), (23, 15), 43, 0, 360, (255, 255, 255), -1)
    YCRCB = cv2.cvtColor(img, cv2.COLOR_BGR2YCR_CB)
    (y, cr, cb)= cv2.split(YCRCB)
    skin = np.zeros(cr.shape, dtype=np.uint8)
    (x, y)= cr.shape
    for i in range(0, x):
        for j in range(0, y):
            CR= YCRCB[i, j, 1]
            CB= YCRCB[i, j, 2]
            if skinCrCbHist [CR, CB] > 0:
                skin[i, j]= 1
    return skin

def SkinColormask2(img):
    skin = np.zeros((img.shape[0], img.shape[1]))
    for y in range(img.shape[0]):
        for x in range(img.shape[1]):  
            B = img.item(y, x, 0)
            G = img.item(y, x, 1)
            R = img.item(y, x, 2)
                    
            if (abs(R - G) > 15) and (R > G) and (R > B):
                if (R > 95) and (G > 40) and (B > 20) and (max(R,G,B) - min(R,G,B) > 15):               
                    skin[y, x] = 1    
                elif (R > 220) and (G > 210) and (B > 170):
                    skin[y, x] = 1
    return skin.astype("int8")

def FaceMask(img, landmarks_all): # use 81 landmarks to define the forehead
    contours = []
    for landmarks in landmarks_all:
        for i in range(16):
            contours.append(landmarks[i])
        for i in [78, 74, 79, 73, 72, 80, 71, 70, 69, 68, 76, 75, 77] :
            contours.append(landmarks[i])

    mask = np.zeros(img.shape[0 : 2], dtype="uint8")
    cv2.fillPoly(mask, np.int32([contours]), 1)
    # plt.imshow(mask) # seems as if the list should be in the correct order
    # plt.show()

    return mask

def BackgroundMask(img):
    mask = np.zeros(img.shape[:2], np.uint8)

    bgdModel = np.zeros((1, 65), np.float64)
    fgdModel = np.zeros((1, 65), np.float64)

    rect = (1, 1, img.shape[1], img.shape[0])
    cv2.grabCut(img, mask, rect, bgdModel, fgdModel, 10, cv2.GC_INIT_WITH_RECT)

    mask = np.where((mask==2)|(mask==0), 1, 0).astype('uint8')
    # plt.imshow(mask)
    # plt.show()
    return mask
    
def WhitenFuntion(img, landmarks_all):
    # get the areas with skin color
    skinmask = SkinColormask2(img)
    facemask = FaceMask(img, landmarks_all)
    backmask = BackgroundMask(img)
    mask = (facemask | (skinmask))  & (~backmask)  
    # mask = np.ones((img.shape[0], img.shape[1]))
    # plt.imshow(mask)
    # plt.show()

    # change skin color
    # imgtemp = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    # for x in range(img.shape[1]):
    #     for y in range(img.shape[0]):
    #         if mask[y, x] == 1:
    #             imgtemp[y, x, 2] = color_transform_list[imgtemp[y, x, 2]] 
    # imgtemp = cv2.cvtColor(imgtemp, cv2.COLOR_HSV2BGR)
    imgtemp = img.copy()
    for x in range(img.shape[1]):
        for y in range(img.shape[0]):
            if mask[y, x] == 1:
                imgtemp[y, x, 0] = color_transform_list[img[y, x, 0]] # B
                imgtemp[y, x, 1] = color_transform_list[img[y, x, 1]] # G
                imgtemp[y, x, 2] = color_transform_list[img[y, x, 2]] # R
                if imgtemp[y, x, 2] < imgtemp[y, x, 1] and imgtemp[y, x, 2] < imgtemp[y, x, 0]: # blue
                    imgtemp[y, x, 2]  = img[y, x, 2]
    cv2.imshow('1', imgtemp)

    # skin vague
    filtered = cv2.bilateralFilter(imgtemp, 9, 75, 75)
    for i in range(3):
        filtered[:, :, i] = filtered[:, :, i]  * mask
    cv2.imshow('2', filtered)
    
    # merge filtered with imgtemp
    merged = np.where(filtered == 0, imgtemp, np.floor((0.65 * filtered + 0.35 * imgtemp)).astype(np.uint8))
    # merged = np.floor((0.65 * filtered + 0.35 * imgtemp)).astype(np.uint8)
    cv2.imshow('3', merged)

    # sharpen
    a = cv2.cvtColor(merged, cv2.COLOR_BGR2RGB)
    image = Image.fromarray(a)
    sharpened = image.filter(ImageFilter.SHARPEN)
    imgout = cv2.cvtColor(np.asarray(sharpened), cv2.COLOR_RGB2BGR) 
    # imgout = merged

    return imgout

def main():
    # # read in original
    img_name = "10"
    img = cv2.imread(img_name + '.jpg')
    cv2.imshow('src', img)
    landmarks_all = landmark_dec_dlib_fun(img)

    # big eyes
    img = BigEyesFunction(img)
    cv2.imshow('eyebigger', img)
    cv2.imwrite(img_name + 'eyesbigger.jpg', img)

    # lighten eyes
    img = LightEyesFunction(img)
    cv2.imshow('eyelight', img)
    cv2.imwrite(img_name + 'eyeslighter.jpg', img)

    # shrink cheeks
    img, landmarks_all = ShrinkFaceFunction(img) # after shape changing, no need to relocate lmks
    cv2.imshow('faceshrink', img)
    cv2.imwrite(img_name + 'faceshrink.jpg', img)

    # whiten 
    img = WhitenFuntion(img, landmarks_all)
    cv2.imshow('facewhite', img)
    cv2.imwrite(img_name + 'facewhite.jpg', img)

    cv2.waitKey(0)
 
if __name__ == '__main__':
    main()