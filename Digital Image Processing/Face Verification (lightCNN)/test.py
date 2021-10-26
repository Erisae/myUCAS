from __future__ import print_function
import argparse
import os

import torch
# from torch._C import TensorType
import torch.nn.parallel
import torch.backends.cudnn as cudnn
import torch.optim
import torch.utils.data
import torchvision.transforms as transforms
import cv2
from PIL import Image


from utils.light_cnn import LightCNN_9Layers, LightCNN_29Layers
from utils.resnet import ResNet50
from utils.triplet_loss import TripletLoss

parser = argparse.ArgumentParser(description='PyTorch Light CNN Training')
parser.add_argument('--arch', '-a', metavar='ARCH', default='LightCNN')
parser.add_argument('--cuda', '-c', default=True)
parser.add_argument('-j', '--workers', default=16, type=int, metavar='N',
                    help='number of data loading workers (default: 1)')
parser.add_argument('--epochs', default=80, type=int, metavar='N',
                    help='number of total epochs to run')
parser.add_argument('--start-epoch', default=0, type=int, metavar='N',
                    help='manual epoch number (useful on restarts)')
parser.add_argument('-b', '--batch-size', default=2, type=int,
                    metavar='N', help='mini-batch size (default: 10)')
parser.add_argument('--momentum', default=0.9, type=float, metavar='M',
                    help='momentum')
parser.add_argument('--weight-decay', '--wd', default=1e-4, type=float,
                    metavar='W', help='weight decay (default: 1e-4)')
parser.add_argument('--print-freq', '-p', default=100, type=int,
                    metavar='N', help='print frequency (default: 100)')
parser.add_argument('--model', default='', type=str, metavar='Model',
                    help='model type: LightCNN-9, LightCNN-29, LightCNN-29v2')
parser.add_argument('--resume', default='', type=str, metavar='PATH',
                    help='path to latest checkpoint (default: none)')
parser.add_argument('--root_path', default='', type=str, metavar='PATH',
                    help='path to root path of images (default: none)')
parser.add_argument('--img_list', default='', type=str, metavar='PATH',
                    help='path to image list (default: none)')
parser.add_argument('--save_path', default='', type=str, metavar='PATH',
                    help='path to save checkpoint (default: none)')
parser.add_argument('--num_classes', default=99891, type=int,
                    metavar='N', help='number of classes (default: 99891)')


def main():

    global args
    args = parser.parse_args()

    if args.model == 'LightCNN-9':
        model = LightCNN_9Layers(num_classes=args.num_classes)
    elif args.model == 'LightCNN-29':
        model = LightCNN_29Layers(num_classes=args.num_classes)
    elif args.model == 'ResNet-50':
        model = ResNet50(num_classes=args.num_classes)
    else:
        print('Error model type\n')

    if args.cuda:
        model = torch.nn.DataParallel(model).cuda()

    if args.resume:
        if os.path.isfile(args.resume):
            print("=> loading checkpoint '{}'".format(args.resume))
            checkpoint = torch.load(args.resume)
            args.start_epoch = checkpoint['epoch']
            model.load_state_dict(checkpoint['state_dict'])
            print("=> loaded checkpoint '{}' (epoch {})"
                  .format(args.resume, checkpoint['epoch']))
        else:
            print("=> no checkpoint found at '{}'".format(args.resume))

    cudnn.benchmark = True
    criterion = TripletLoss()

    if args.cuda:
        criterion.cuda()

    # evaluate on validation set
    test(args.img_list, model, criterion)


def pred(inputs, margin = 0.003):
    n = inputs.size(0)   
    dist = torch.pow(inputs, 2).sum(dim=1, keepdim=True).expand(n, n) 
    dist = dist + dist.t()      
    dist.addmm_(1, -2, inputs, inputs.t())    
    dist = dist.clamp(min=1e-12).sqrt() 

    outputs = torch.where(dist.cpu() >= margin, torch.tensor(0).cpu(), torch.tensor(1).cpu()) 
    return outputs


def test(filelist, model, criterion):
    transform=transforms.Compose([ 
                transforms.CenterCrop(112),
                transforms.ToTensor(),
            ])

    # switch to evaluate mode
    model.eval()
    torch.no_grad()
    size = (112, 112)
    for margin in [0.0080]:
        with open(filelist, 'r') as file:
            with open("/home/yhxia/DIP-competition-testdata/answer2.txt", "w") as tf:
                for line in file.readlines():
                    imgPath1, imgPath2= line.strip().split(' ')
                    im1 = cv2.resize(cv2.imread(os.path.join(args.root_path, imgPath1)), size, interpolation=cv2.INTER_AREA)
                    im2 = cv2.resize(cv2.imread(os.path.join(args.root_path, imgPath2)), size, interpolation=cv2.INTER_AREA)
                    img1 = transform(Image.fromarray(im1))
                    img2 = transform(Image.fromarray(im2))

                    input = torch.zeros((2, 3, 112, 112))
                    input[0, :, :, :] = img1
                    input[1, :, :, :] = img2
                    input      = input.cuda()

                    output = model(input)
                    label_pred = pred(output, margin = margin)
                    tf.writelines(str(int(label_pred[0, 1].data)))
                    tf.writelines("\n")


if __name__ == '__main__':

    main()



