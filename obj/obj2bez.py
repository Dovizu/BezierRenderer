import sys

def load_obj_file(filename):
    """Loads a Wavefront OBJ file. """
    """Parser credit: pygame """
    vertices = [0]
    normals = [0]
    #texcoords = [0]
    faces = [0]
    max_val = 0.0

    #material = None
    for line in open(filename, "r"):
        if line.startswith('#'): continue
        values = line.split()
        if not values: continue
        if values[0] == 'v':
            v = map(float, values[1:4])
            vertices.append(v)
            max_val = max(max_val, max(v))
        elif values[0] == 'f':
            face = []
            for v in values[1:]:
                w = v.split('/')
                face.append(int(w[0]))
            faces.append(face);

    #Renormalize
    if max_val > 3.0:
      ratio = 3.0/max_val
      new_vert = [0] + [[ratio*v for v in vert] for vert in vertices[1:]]
      vertices = new_vert

    new_filename = filename[:-3] + "bez"
    out = open(new_filename, 'w+')
    out.write(str(len(faces[1:]))+'\n')
    for face in faces[1:]:
        v1 = vertices[face[0]]
        v2 = vertices[face[1]]
        v3 = vertices[face[2]]
        if len(face) == 4:
            v4 = vertices[face[3]]
        else:
            v4 = v3
        # if set(v4) == set(v3):
        #     x = (v3[0]-v2[0])/2
        #     y = (v3[1]-v2[1])/2
        #     z = (v3[2]-v2[2])/2
        #     v3[0] -= x
        #     v3[1] -= y
        #     v3[2] -= z

        s1 = v_to_s(v1) + v_to_s(v1) + v_to_s(v2) + v_to_s(v2) + '\n'
        s2 = v_to_s(v4) + v_to_s(v4) + v_to_s(v3) + v_to_s(v3) + '\n'

        out.write(s1)
        out.write(s1)
        out.write(s2)
        out.write(s2)
        out.write('\n')
    out.close()

def v_to_s(v):
    """ v = [a, b, c] """
    return ' ' + str(v[0]) + ' ' + str(v[1]) + ' ' + str(v[2]) + ' '

def vecSub(u, v):
    return [v[0]-u[0], v[1]-u[1], v[2]-u[2]]

def vecAdd(u, v):
    return [v[0]+u[0], v[1]+u[1], v[2]+u[2]]

def vecDiv(u, scalar):
    return [u[0]/scalar, u[1]/scalar, u[2]/scalar]

def vecMul(u, scalar):
    return [u[0]*scalar, u[1]*scalar, u[2]*scalar]

def main(argv):
    if len(argv) < 2:
        print "Provide an input filename"
        sys.exit(2)
    load_obj_file(argv[1])



main(sys.argv)



