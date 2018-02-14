from freetype import *
from shapely.geometry import Polygon

face = Face('DejaVuSerif.ttf')
flags = FT_LOAD_DEFAULT | FT_LOAD_NO_BITMAP
face.set_char_size( 32*64  )
face.load_char(sys.argv[1], flags )
slot = face.glyph
outline = slot.outline
print(Polygon(outline.points).wkt)
