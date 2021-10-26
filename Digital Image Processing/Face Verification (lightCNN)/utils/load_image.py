import torch.utils.data as data
import cv2

from PIL import Image
import os
import os.path

def default_list_reader(filename):
    imgList = []
    with open(filename, 'r') as file:
        for line in file.readlines():
            path, label = line.strip().split(' ')
            imgList.append((path, int(label))) # label is a num
    return imgList

def default_loader(path):
    img = cv2.imread(path)
    return img


class ImageList(data.Dataset):

    def __init__(self, rootpath, filename, transform = None, list_reader = default_list_reader, loader = default_loader):
        self.rootpath      = rootpath
        self.imgList       = list_reader(filename)
        self.transform     = transform
        self.loader        = loader

    def __getitem__(self, index):
        path, label = self.imgList[index]
        img = self.loader(os.path.join(self.rootpath, path))

        if self.transform is not None:
            img = self.transform(Image.fromarray(img))
        return img, label

    def __len__(self):
        return len(self.imgList)