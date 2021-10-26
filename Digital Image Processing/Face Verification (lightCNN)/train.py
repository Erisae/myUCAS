from __future__ import print_function
import argparse
import os

import torch
# from torch._C import TensorType
import torch.nn.parallel
import torch.backends.cudnn as cudnn
import torch.optim
import torch.utils.data
from torch.utils.data.sampler import BatchSampler, SubsetRandomSampler, SequentialSampler
import torchvision.transforms as transforms

import numpy as np

from utils.light_cnn import LightCNN_9Layers, LightCNN_29Layers
from utils.resnet import ResNet50
from utils.load_image import ImageList
from utils.triplet_loss import TripletLoss


import pdb

############################################################################
########################### parser definition ############################## 

parser = argparse.ArgumentParser(description='Face verification Training')
parser.add_argument('--arch', '-a', metavar='ARCH', default='LightCNN')
parser.add_argument('--cuda', '-c', default=True)
parser.add_argument('-j', '--workers', default=16, type=int, metavar='N',
                    help='number of data loading workers (default: 1)')
parser.add_argument('--epochs', default=240, type=int, metavar='N',
                    help='number of total epochs to run')
parser.add_argument('--start-epoch', default=0, type=int, metavar='N',
                    help='manual epoch number (useful on restarts)')
parser.add_argument('-b', '--batch-size', default=30, type=int,
                    metavar='N', help='mini-batch size (default: 10)')
parser.add_argument('--lr', '--learning-rate', default=0.0001, type=float,
                    metavar='LR', help='initial learning rate')
parser.add_argument('--weight-decay', '--wd', default=1e-4, type=float,
                    metavar='W', help='weight decay (default: 1e-4)')
parser.add_argument('--print-freq', '-p', default=100, type=int,
                    metavar='N', help='print frequency (default: 100)')
parser.add_argument('--model', default='', type=str, metavar='Model',
                    help='model type: LightCNN-9, LightCNN-29, ResNet-50')
parser.add_argument('--resume', default='', type=str, metavar='PATH',
                    help='path to latest checkpoint (default: none)')
parser.add_argument('--root_path', default='', type=str, metavar='PATH',
                    help='path to root path of images (default: none)')
parser.add_argument('--train_list', default='', type=str, metavar='PATH',
                    help='path to training list (default: none)')
parser.add_argument('--val_list', default='', type=str, metavar='PATH',
                    help='path to validation list (default: none)')
parser.add_argument('--save_path', default='', type=str, metavar='PATH',
                    help='path to save checkpoint (default: none)')
parser.add_argument('--num_classes', default=100000, type=int,
                    metavar='N', help='number of classes (default: 100000)')

###########################################################################
########################### control definition ############################

def adjust_learning_rate(optimizer, epoch):
    scale = 0.5
    step  = 12
    lr = args.lr * (scale ** (epoch // step))
    print('lr: {}'.format(lr))
    if (epoch != 0) & (epoch % step == 0):
        print('Change lr')
        for param_group in optimizer.param_groups:
            param_group['lr'] = param_group['lr'] * scale

def save_checkpoint(state, filename):
    torch.save(state, filename)


class AverageMeter(object):
    """Computes and stores the average and current value"""
    def __init__(self):
        self.val   = 0
        self.avg   = 0
        self.sum   = 0
        self.count = 0

    def update(self, val, n=1):
        self.val   = val
        self.sum   += val * n
        self.count += n
        self.avg   = self.sum / self.count

############################################################################
########################### training definition ############################

def train(train_loader, model, criterion, optimizer, epoch):
    losses     = AverageMeter()
    prec1es    = AverageMeter()
    pavgs      = AverageMeter()
    navgs      = AverageMeter()
    psds       = AverageMeter()
    nsds       = AverageMeter()

    model.train()

    for i, (input, target) in enumerate(train_loader):

        input      = input.cuda().type(torch.FloatTensor)
        target     = target.cuda().type(torch.FloatTensor)
        input_var  = torch.autograd.Variable(input, requires_grad = True)  # torch.autograd.Variable
        target_var = torch.autograd.Variable(target, requires_grad = True)

        # compute output
        output = model(input_var)
        loss, prec1, pavg , navg, psd, nsd = criterion(output, target_var)

        # measure accuracy and record loss
        losses.update(loss.data, input.size(0)) 
        prec1es.update(prec1.data, input.size(0))
        pavgs.update(pavg.data, input.size(0))
        navgs.update(navg.data, input.size(0))
        psds.update(psd.data, input.size(0))
        nsds.update(nsd.data, input.size(0))

        # compute gradient and do ADAM step
        optimizer.zero_grad()
        loss.backward()
        optimizer.step()

        if i % args.print_freq == 0:
            print('Epoch: [{0}][{1}/{2}] '
                  'Loss {loss.val:.4f} ({loss.avg:.4f}) '
                  'Prec {prec1.val:.4f} ({prec1.avg:.4f}) '
                  'Pa {pavg.val:.6f} ({pavg.avg:.6f}) '
                  'Na {navg.val:.6f} ({navg.avg:.6f}) '
                  'Ps {psd.val:.6f} ({psd.avg:.6f}) '
                  'Ns {nsd.val:.6f} ({nsd.avg:.6f})'
                  .format(
                   epoch, i, len(train_loader), loss=losses, prec1 = prec1es, pavg=pavgs, navg=navgs, psd = psds, nsd = nsds))

    return (pavgs.avg + navgs.avg) / 2.0


############################################################################
########################### validating definition ##########################

def pred(inputs, margin = 0.0102):
    n = inputs.size(0)   
    dist = torch.pow(inputs, 2).sum(dim=1, keepdim=True).expand(n, n) 
    dist = dist + dist.t()      
    dist.addmm_(1, -2, inputs, inputs.t())    
    dist = dist.clamp(min=1e-12).sqrt() 
    if not torch.is_tensor(margin):
        margin = torch.tensor(margin)
    outputs = torch.where(dist.cpu().double() >= margin.cpu().double(), torch.tensor(0).cpu().double(), torch.tensor(1).cpu().double()) 
    return outputs

def my_accuracy(pred, gt):
    n = pred.size(0)
    gt = gt.expand(n, n)
    gt = gt.eq(gt.t())
    eqq = gt.eq(pred.cuda().byte()).cpu()  # 1 means correct and o means incorrect
    sum = (eqq.sum() - n).type(torch.DoubleTensor)
    mul = torch.tensor(n * n - n).type(torch.DoubleTensor)
    acc = sum / mul * torch.tensor(100).type(torch.DoubleTensor)

    return acc

def validate(val_loader, model, criterion, margin = 0.0102):
    losses     = AverageMeter()
    acces      = AverageMeter()
    preces     = AverageMeter()
    pavgs      = AverageMeter()
    navgs      = AverageMeter()
    psds       = AverageMeter()
    nsds       = AverageMeter()

    # switch to evaluate mode
    model.eval()
    with torch.no_grad():
        for i, (input, target) in enumerate(val_loader):
            input      = input.cuda()
            target     = target.cuda()
            input_var  = torch.autograd.Variable(input)
            target_var = torch.autograd.Variable(target)

            output = model(input_var)
            loss, prec1, pavg, navg, psd, nsd  = criterion(output, target_var)

            label_pred = pred(output, margin = margin)
            acc = my_accuracy(label_pred, target_var)

            acces.update(acc.data, input.size(0))
            losses.update(loss.data, input.size(0))
            preces.update(prec1.data, input.size(0))
            pavgs.update(pavg.data, input.size(0))
            navgs.update(navg.data, input.size(0))
            psds.update(psd.data, input.size(0))
            nsds.update(nsd.data, input.size(0))

    print('\nTest set: Ave loss: {}, Acc: {:.4f}, Prec: {:.4f}, AVG_P : {:.6f}, AVG_N : {:.6f}, SD_P : {:.6f}, SD_N : {:.6f}, margin : {}\n'
                                .format(losses.avg, acces.avg, preces.avg, pavgs.avg, navgs.avg, psds.avg, nsds.avg , margin))

    return acces.avg, losses.avg

############################################################################
################################ main #######3############################## 

def main():
    global args
    args = parser.parse_args()

    #############   create model for training #################
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


    # =============  large lr for last fc parameters ================
    params = []
    for name, value in model.named_parameters():
        if 'bias' in name:
            if 'fc2'or 'fc3' or 'fc4' in name:
                params += [{'params':value, 'lr': 20 * args.lr, 'weight_decay': 0}]
            else:
                params += [{'params':value, 'lr': 2 * args.lr, 'weight_decay': 0}]
        else:
            if 'fc2' in name:
                params += [{'params':value, 'lr': 10 * args.lr}]
            else:
                params += [{'params':value, 'lr': 1 * args.lr}]


    # ================ define the adam optimizer =====================
    optimizer = torch.optim.Adam(params, args.lr,
                                weight_decay=args.weight_decay)


    # ================= resume from a checkpoint ======================
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


    # ==================== define data loader ==========================
    train_loader = torch.utils.data.DataLoader(
        ImageList(root=args.root_path, fileList=args.train_list, 
            transform=transforms.Compose([ 
                transforms.RandomCrop(112),
                transforms.RandomHorizontalFlip(), 
                transforms.ToTensor(),
            ])),
        batch_sampler = BatchSampler(
            SubsetRandomSampler(
                list(BatchSampler(
                    SequentialSampler(range(37500)), 
                    batch_size= 5, 
                    drop_last=True))), 
            batch_size = 2, 
            drop_last = True),
        num_workers=args.workers, pin_memory=True)

    val_loader = torch.utils.data.DataLoader(
        ImageList(root=args.root_path, fileList=args.val_list, 
            transform=transforms.Compose([ 
                transforms.CenterCrop(112),
                transforms.ToTensor(),
            ])),
        batch_sampler = BatchSampler(
            SubsetRandomSampler(
                list(BatchSampler(
                    SequentialSampler(range(12500)), 
                    batch_size= 5, 
                    drop_last=True))), 
            batch_size = 2, 
            drop_last = True),
        num_workers=args.workers, pin_memory=True)   
    
    # ====================== define the criterion for training ================
    criterion = TripletLoss()

    if args.cuda:
        criterion.cuda()

    # validate(val_loader, model, criterion) 

    prec = []
    loss = []   

    for epoch in range(args.start_epoch, args.epochs):

        # ================== adjust learning rate ==============================
        adjust_learning_rate(optimizer, epoch)

        # ================== train and validate ================================
        margin = train(train_loader, model, criterion, optimizer, epoch)
        prec1, loss1 = validate(val_loader, model, criterion, margin = margin)

        # ================== save checkpoint ===================================
        save_name = args.save_path + 'LightCNNzzz_' + str(epoch+1) + '_checkpoint.pth.tar'
        save_checkpoint({
            'epoch': epoch + 1,
            'arch': args.arch,
            'state_dict': model.state_dict(),
            'prec1': prec1,
        }, save_name)

        loss.append(loss1)
        prec.append(prec1)

    # ===================== save loss and acc ====================================
    np.save("loss.npy", np.array(loss))
    np.save("prec.npy", np.array(prec))


if __name__ == '__main__':
    main()