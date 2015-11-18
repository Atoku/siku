import h5py

f = h5py.File('save_test.h5', 'r')
d = f['Monitor functions']
[print('|'+l.decode()+'|') for l in d ]
print('\n')

es = f['Elements/Elements']

[ print(e) for e in es[:1] ]
e = es[0]
print(len(es.dtype))
print(len(e))
print(e[5])
print(es.dtype[5])
