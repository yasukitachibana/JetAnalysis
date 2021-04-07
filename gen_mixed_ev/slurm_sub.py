import os

def GetSbatchCmd(cmd, dir, que, name, mem):
  
  head = 'sbatch -q {} --time {} --mem {} --job-name {} -o {} -e {} '
  head = head.format(que, '2-0', mem, name, name+'_out.txt', name+'_err.txt')
  #print( "#slurm sub: ", head)
  master = os.path.join(dir,'JobMaster') + ' "' + dir + '" "' + cmd +'"'
  #print( "#slurm sub: ", master)
  return head+master
