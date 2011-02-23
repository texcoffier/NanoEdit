camera {
location <3,0.5,-3.5>
sky <0.0,0.0,-1.0>
right x
up y
look_at <0,0,-0>}
union {
union {
light_source {<1,1.5,2.5> color rgb <1,1,1>}
light_source {<3,-4,-2> color rgb <1,0,0>}
union {
intersection {
union {
sphere { <0, 0, 0>, 1 }
scale <1.05,1.05,1.05>
translate <0,0,0.5>
}
difference {
union {
cylinder { <0, 0, 0>, <0, 0, 1>, 1 }
union {
box { <0, 0, 0>, <1, 1, 1> }
translate <-0.5,-0.5,1>
}
union {
cone { <0, 0, 0>, 0, <0, 0, 1>, 1 }
translate <0,0,-1>
scale <1.3,1.3,1.3>
}
}
union {
box { <0, 0, 0>, <1, 1, 1> }
translate <0,0,-0.5>
scale <5,5,5>
}
}
}
pigment {red 1 green 1 blue 1}
finish { diffuse 0.8
         phong 0.1
         reflection 0.1
         refraction 0
         ambient rgb <0.1,0.1,0.1>
         phong_size 4  
         ior 2 }
}
}
matrix <1, 0, 0,  0, 1, 0,  0, 0, -1,  0, 0, 0>
}