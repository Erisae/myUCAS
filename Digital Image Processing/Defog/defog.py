import cv2
import math
import numpy as np
from PIL import Image
 
def GenDC(img, size = 10):
    '''
    首先对图像每个像素取三通道中最小值，得到一个单通道图，
    然后对这个单通道图作最小值滤波就可以得到暗通道图
    '''
    one_channel = cv2.min(cv2.min(img[:,:,2], img[:,:,1]), img[:,:,0])
    kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (size, size))
    dc = cv2.erode(one_channel, kernel) # cv2.erode进行腐蚀
    return dc
 
def GenA(img, dc):
    '''
    在暗通道中找出前0.1%最亮的点，即透射率最小的点。
    对于这些点，去雾图中找到对应的点，并取它们中的所有通道最大的值作为A的近似。
    '''
    # number of 1%% light
    num = math.floor(img.size / 3 * 0.001)
    if num == 0:
        num = 1

    selected = dc.reshape(-1, 1).argsort(axis = 0)[int(img.size / 3) - num : , :]

    # # way 1 find max
    # Aout = np.array([-1, -1, -1]).astype(float)
    # for i in range(num):
    #     for j in range(3):
    #         x = img.reshape(-1, 3)[selected[i, 0], :][j]
    #         Aout[j] = max(x, Aout[j])
    
    # way 2 find mean
    Aout = []
    for i in range(0, num):
        Aout.append(img.reshape(-1, 3)[selected[i, :], :])
    Aout = np.array(Aout).mean(axis = 0)

    return Aout.reshape(-1, 3)

def GuidedFilter(p, I, r = 60, reg = 0.001):
    '''
    p : input image to be filtered
    I : guidance image
    r : radius
    reg : regularization
    '''
    mean_I = cv2.boxFilter(I, cv2.CV_64F, (r, r))
    mean_p = cv2.boxFilter(p, cv2.CV_64F, (r, r))
    corr_I = cv2.boxFilter(I * I, cv2.CV_64F, (r, r))
    corr_Ip = cv2.boxFilter(I * p, cv2.CV_64F, (r, r))

    var_I   = corr_I - mean_I * mean_I
    cov_Ip = corr_Ip - mean_I * mean_p
 
    a = cov_Ip / (var_I + reg)
    b = mean_p - a * mean_I
 
    mean_a = cv2.boxFilter(a, cv2.CV_64F, (r, r))
    mean_b = cv2.boxFilter(b, cv2.CV_64F, (r, r))
 
    q = mean_a * I + mean_b

    return q

 
def GenT(img_ori, img, A, omega = 0.95, size = 10):

    ratio = np.zeros(img.shape)
    for i in range(0, 3):
        ratio[:, :, i] = img[:, :, i] / A[0, i]
 
    Tout = 1 - omega * GenDC(ratio)

    gray = cv2.cvtColor(img_ori, cv2.COLOR_BGR2GRAY)/ 255.
    Tout = GuidedFilter(Tout, gray)
    # Tout += 0.25

    return Tout
 
 
def GenDefog(img, A, T, t0 = 0.6):
    out = np.zeros(img.shape)
    T = cv2.max(T, t0)
    for i in range(3):
        out[:, :, i] = (img[:, :, i] - A[0, i]) / T + A[0, i]
    
    out = out
    return out

def GenLight(img, gamma = 0.75):
    defog = np.power(img, gamma)
    
    return defog * 255

def main():
    name = "train"
    filename = 'E:\\UcasCourse\\数字图像处理\\去雾\\去雾算法测试图像\\Org images\\' + name + ".bmp"
    img = Image.open(filename)
    img1 = cv2.cvtColor(np.asarray(img), cv2.COLOR_RGB2BGR)
    img2 = img1.astype('float64') / 255.

    DC = GenDC(img2)
    A = GenA(img2, DC)
    T = GenT(img1, img2, A)
    cv2.imwrite(name + "_" + "my_t.png", (T * 255).astype("uint8"))
    defog = GenDefog(img2, A, T)
    defog = GenLight(defog)
    cv2.imwrite(name + "_" + "my_defog.png", defog)

 
if __name__ == '__main__':
    main()