
f = open('Information_Computer_Data_DNASkittle_businesscard_bcbanner.fa', 'r')
o = open('duplicated.fasta', 'w')
seq = f.read()
lineSize = 74 * 2
for start in range(0,len(seq), lineSize):
   sub = seq[start:start+lineSize]
   newline = ''
   for ch in sub:
      newline += ch + ch
   o.write(newline)
   o.write(newline)
o.close()
f.close()
