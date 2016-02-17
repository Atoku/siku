'''File: noisy_grid.py
Module: noisy grid - simple utility for generationg rectangular grid
of quadrilaterals with vertices shifted by some random 'noise'
'''

import random

def shift( x, y, dx, dy = None ):
    '''Displace x and y values within +/-dx and +/-dy ranges respectively

    Return: (xx, yy) - displaced point
    '''
    if dy == None:
        dy = dx

    xx = random.uniform( x - dx, x + dx )
    yy = random.uniform( y - dy, y + dy )

    return (xx, yy)

def make_poly( verts ):
    '''Convert a table (list of lists MxN) of vertice into
    a set (list of lists of points) of polygons.

    IMPORTANT! polygons are formed with the assumption that a list is
    numerated from the bottm-left corner:
    |  21  22  |
    |          |   <=>  [ [ 11, 12 ], [ 21, 22 ] ]
    |  11  12  |
    If so - the output will be CCW-oriented 4-edge polygons

    Return: [ [ (x, y) ] ]
    '''

    res = []
    I = len(verts)
    J = len(verts[0])

    for i in range(I - 1):
        for j in range(J - 1):
            p = [ verts[i][j], verts[i][j+1],
                  verts[i+1][j+1], verts[i+1][j] ]
            res.append( p )

    return res

def generate( left, bottom, right, top, nx, ny, dx, dy ):
    '''Main method - generates a noisy grid, in which:
    Xi = left + i*(right-left)/nx  -/+ dx
    Yj = bottom + j*(top-bottom)/ny  -/+ dy

    Params:
    left, bottom, right, top - boarders of geterating area
    nx, ny - amounts of lines and coloumns ( >= 1)
    dx, dy - displacement ranges (relative value) 

    Return: [ [ (x, y) ] ]
    '''
    
    stepx = (right-left)/nx
    stepy = (top-bottom)/ny

    verts = []
    for j in range(ny+1):
        line = []
        for i in range(nx+1):
            x = left+i*stepx
            y = bottom+j*stepy
            line.append( shift(x, y, stepx*dx, stepy*dy) )
            
        verts.append( line )

    coords = make_poly( verts )

    return coords
        
## ------------------------------- main --------------------------------------

def main():
    ''' will ask for input, generate gid and storage it inside
    the list named 'coords'
    '''
    
    t = input('put left, bottom, right and top edges: ')
    left, bottom, right, top = tuple([ float(x) for x in t.split()])
    
    t = input('put the amount of lines and columns (nx, ny): ')
    nx, ny = tuple([ int(x) for x in t.split()])

    t = input('put the displacements` ranges (dx, dy): ')
    dx, dy = tuple([ float(x) for x in t.split()])

    coords = generate( left, bottom, right, top, nx, ny, dx, dy )

    b = input('print? (y/n): ')
    if b == 'y':
        for l in coords:
            t = '[ '
            for w in l:
                t = t + '( ' + str(w[0]) + ', ' + str(w[1]) + ' ) '
            t = t + ']'
            print( t )
            
if __name__ == '__main__':
    main()
