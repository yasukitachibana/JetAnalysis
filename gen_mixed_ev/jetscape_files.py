##############################################################################################
def ParticleFileName(hadpart, ptHatMin, ptHatMax):
  return 'Jetscape{}ListBin{}_{}.out'.format(str(hadpart),str(ptHatMin), str(ptHatMax))

def SigmaFileName(ptHatMin, ptHatMax):
  return 'SigmaHardBin{}_{}.out'.format(str(ptHatMin), str(ptHatMax))

def PpDir():
  return '/wsu/home/go/go54/go5410/maj-shen/AAPaperData/5020_PP_Colorless'
##############################################################################################
def MixedEventFileName(hadpart, index=''):
  return 'MixedEvent{}.txt'.format(str(hadpart)+str(index))
##############################################################################################


##############################################################################################
def PtHatBins(ecm):
  
  if ecm == 5020:
    
    pthat_bins = [ [1,2], [2,3], [3, 4], [4, 5], [5,7], [7,9], [9,11], [11,13], [13,15], [15,17], [17,20], [20,25],
                  [25, 30], [30, 35], [35, 40], [40, 45], [45, 50], [50, 55], [55, 60], [60, 70], [70,80], [80,90], [90, 100],
                  [100, 110], [110, 120], [120, 130], [130, 140], [140, 150], [150, 160], [160, 170], [170, 180], [180, 190], [190, 200],
                  [200, 210], [210, 220], [220, 230], [230, 240], [240, 250], [250, 260], [260, 270], [270, 280], [280, 290], [290, 300],
                  [300, 350], [350, 400], [400, 450], [450, 500], [500, 550], [550, 600], [600, 700], [700, 800], [800, 900], [900, 1000],
                  [1000,1100], [1100,1200], [1200,1300], [1300,1400], [1400,1500], [1500,1600], [1600,1700], [1700,1800], [1800,1900], [1900,2000],
                  [2000,2200], [2200,2400], [2400,2510] ]
      
    return pthat_bins
  else:
    print('Error')
    exit()
##############################################################################################
