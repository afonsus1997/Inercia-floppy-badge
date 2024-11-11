import numpy as np
import BlueNoise as bn


BAYER_LEVELS         = 5
WHITE_NOISE_VARIANTS = 2
BLUE_NOISE_VARIANTS  = 2
WHITE_NOISE_SIZE     = 64    
BLUE_NOISE_SIZE      = 64    


def normalize(map_):
    return (map_ / (map_.shape[0] * map_.shape[1]) * 255.0).astype(np.uint8)


def gen_bayer_map():
    bayerm = np.array(((0, 2), (3, 1)))
    yield normalize(bayerm)

    while True:
        bayerm = np.vstack((np.hstack((bayerm * 4 + 0, bayerm * 4 + 2)),
                            np.hstack((bayerm * 4 + 3, bayerm * 4 + 1))))
        yield normalize(bayerm)

maps = []

maps.append(('CONSTANT', np.array([127,])))

bmgen = gen_bayer_map()
for i in range(BAYER_LEVELS):
    maps.append((f'BAYER{i}', next(bmgen)))


rng = np.random.default_rng(seed=694201337)
for i in range(WHITE_NOISE_VARIANTS):
    wn = rng.normal(size=(WHITE_NOISE_SIZE, WHITE_NOISE_SIZE))
    wn = (wn - np.min(wn)) / (np.max(wn) - np.min(wn))
    wn = (wn * 255.0).astype('uint8')

    maps.append((f'WHITE_NOISE{i}', wn))


for i in range(BLUE_NOISE_VARIANTS):
    bln = bn.GetVoidAndClusterBlueNoise((BLUE_NOISE_SIZE,
                                         BLUE_NOISE_SIZE),
                                        StandardDeviation=1.5)
    bln = normalize(bln)
    maps.append((f'BLUE_NOISE{i}', bln))



INIT_TEMPLATE = \
'''

    {{  /* {name} */

        .size = {size},
        .map = (const uint8_t[]){{
            {hex_bytes}
        }}
    }},

'''

init_list = ''

for map_ in maps:
    bytes_ = map_[1].flatten()
    xbytes = ''

    for i, b in enumerate(bytes_):
        xbytes += f'0x{b:02X}u, '
        if (i + 1) % 8 == 0:
            xbytes += '\n' + ' ' * 12

    init_list += INIT_TEMPLATE.format(name=map_[0],
                                      size=map_[1].shape[0],
                                      hex_bytes=xbytes)


FILE_TEMPLATE = \
'''
#ifndef __TGL_DITHER_MAPS_H
#define __TGL_DITHER_MAPS_H

#include <stdint.h>

/* This file was generated with src/gen_dither_maps/gen.py */


#define NUM_DITHER_MAPS {nmaps}

static const struct
{{
    uint16_t size;
    const uint8_t *map;

}} dither_maps[NUM_DITHER_MAPS] =
{{
    {init_list}
}};


#endif /* __TGL_DITHER_MAPS_H */

'''


FILE_NAME = '../dither_maps.h'

with open(FILE_NAME, 'w') as fl:
    fl.write(FILE_TEMPLATE.format(nmaps=len(maps), init_list=init_list))


print(FILE_NAME, 'generated!')

print('\nMaps enum:\n')

print(f'#define NUM_DITHER_MAPS             {len(maps)}\n')
for i, map_ in enumerate(maps):
    print(f'#define DITHER_MAP_{map_[0]:<16} {i}')

print('\n\n')
