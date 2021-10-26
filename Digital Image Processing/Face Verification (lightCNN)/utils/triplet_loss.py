import torch
import torch.nn as nn


class TripletLoss(nn.Module):
    def __init__(self, margin = 0.004):
        super(TripletLoss, self).__init__()
        self.margin = margin   
        self.loss = nn.MarginRankingLoss(margin=margin)  

    def forward(self, inputs, labels): 
        # =================== calculate dist =========================================
        n = inputs.size(0)  
        dist = torch.pow(inputs, 2).sum(dim=1, keepdim=True).expand(n, n) # n * num_classes, sum 
        dist = dist + dist.t()    
        dist.addmm_(1, -2, inputs, inputs.t())    
        dist = dist.clamp(min=1e-12).sqrt()  

        # ===================== select ap & cn =======================================
        mask = labels.expand(n, n).eq(labels.expand(n, n).t())  
        dist_ap, dist_an = [], []
        for i in range(n):
            dist_ap.append(dist[i][mask[i]].max().view(1))       
            dist_an.append(dist[i][mask[i] == 0].min().view(1))  
        dist_ap = torch.cat(dist_ap)   
        dist_an = torch.cat(dist_an)

        # ===================== calculate avg & std of dist ==========================
        maskp = torch.triu(mask, diagonal=1, out=None).byte()
        maskn = torch.triu(1 - mask, diagonal=1, out=None).byte()

        dist_pavg = dist[maskp].mean()
        dist_navg = dist[maskn].mean()
        dist_psd  = dist[maskp].std()
        dist_nsd  = dist[maskn].std()

        # ===================== calculate loss and prec ==============================
        y = torch.ones_like(dist_an)
        loss = self.loss(dist_an, dist_ap, y)
        prec = (dist_an.data > (dist_ap + self.margin)).data.float().mean() 

        return loss, prec, dist_pavg, dist_navg, dist_psd, dist_nsd