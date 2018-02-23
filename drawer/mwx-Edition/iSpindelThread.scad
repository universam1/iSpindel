$fn=64;

bl=28;   // base length
bh=4;    // base height
  
bw=10;   // outer base width

cl=8;    // cutout length
ch=2;    // cutout height

tl=22;   // thread 

cr=3;    // corner radius

sw=5.2;  // sensor hole width

h=3.5;   // mounting hole diameter
ho=10;   // mounting hole offset

hh=6.5;  // screw head diameter
hd=2.5;  // screw head depth

difference() {
  union() {
    translate([0,0,bh]) thread("M12",tl);
    translate([-bl/2,-bw/2,0]) cube([bl,bw,bh]); 
  }

  translate([0,0,-1]) cylinder(tl+bh+2,sw/2,sw/2);

  translate([-ho,0,-1]) cylinder(bh+2,h/2,h/2);
  translate([ho,0,-1]) cylinder(bh+2,h/2,h/2);

  translate([-ho,0,bh-hd]) cylinder(bh+2,hh/2,hh/2);
  translate([ho,0,bh-hd]) cylinder(bh+2,hh/2,hh/2);

  translate([-(cl/2),-7,-0.01]) cube([cl,14,ch]);
}

module thread(name="M12",length=20){
  df=_get_screw_fam(name);
  lead=df[3];
  orad=df[1]/2;
  _thread(orad,length,lead);
}

module _thread(orad,tl,p) { 
  r = [orad-0/18*p, orad-1/18*p, orad-2/18*p, orad-3/18*p, orad-4/18*p, orad-5/18*p,
    orad-6/18*p, orad-7/18*p, orad-8/18*p, orad-9/18*p, orad-10/18*p, orad-11/18*p,
    orad-12/18*p, orad-13/18*p, orad-14/18*p, orad-15/18*p, orad-16/18*p, orad-17/18*p,
    orad-p];

  translate([0,0,tl/2])
  linear_extrude(height = tl, convexity = 10, twist = -360.0*tl/p, center = true)
    polygon([[ r[ 0]*cos(  0), r[ 0]*sin(  0)], [r[ 1]*cos( 10), r[ 1]*sin( 10)],
      [ r[ 2]*cos( 20), r[ 2]*sin( 20)], [r[ 3]*cos( 30), r[ 3]*sin( 30)],
      [ r[ 4]*cos( 40), r[ 4]*sin( 40)], [r[ 5]*cos( 50), r[ 5]*sin( 50)],
      [ r[ 6]*cos( 60), r[ 6]*sin( 60)], [r[ 7]*cos( 70), r[ 7]*sin( 70)],
      [ r[ 8]*cos( 80), r[ 8]*sin( 80)], [r[ 9]*cos( 90), r[ 9]*sin( 90)],
      [ r[10]*cos(100), r[10]*sin(100)], [r[11]*cos(110), r[11]*sin(110)],
      [ r[12]*cos(120), r[12]*sin(120)], [r[13]*cos(130), r[13]*sin(130)],
      [ r[14]*cos(140), r[14]*sin(140)], [r[15]*cos(150), r[15]*sin(150)],
      [ r[16]*cos(160), r[16]*sin(160)], [r[17]*cos(170), r[17]*sin(170)],
      [ r[18]*cos(180), r[18]*sin(180)], [r[17]*cos(190), r[17]*sin(190)],
      [ r[16]*cos(200), r[16]*sin(200)], [r[15]*cos(210), r[15]*sin(210)],
      [ r[14]*cos(220), r[14]*sin(220)], [r[13]*cos(230), r[13]*sin(230)],
      [ r[12]*cos(240), r[12]*sin(240)], [r[11]*cos(250), r[11]*sin(250)],
      [ r[10]*cos(260), r[10]*sin(260)], [r[ 9]*cos(270), r[ 9]*sin(270)],
      [ r[ 8]*cos(280), r[ 8]*sin(280)], [r[ 7]*cos(290), r[ 7]*sin(290)],
      [ r[ 6]*cos(300), r[ 6]*sin(300)], [r[ 5]*cos(310), r[ 5]*sin(310)],
      [ r[ 4]*cos(320), r[ 4]*sin(320)], [r[ 3]*cos(330), r[ 3]*sin(330)],
      [ r[ 2]*cos(340), r[ 2]*sin(340)], [r[ 1]*cos(350), r[ 1]*sin(350)]
      ]);
}

function _get_screw_fam(n) = data_screw_fam[search([n], data_screw_fam)[0]];

data_screw_fam = [
  // desc,th_outerdia,th_innerdia,lead,key,key_depth,head_height,head_dia,nut_key,nut_height
  ["M1.6", 1.6, "xx", 0.35, 1.5,  0.7, 1.6,   3, 3.2,  1.3],
  ["M2",     2, "xx",  0.4, 1.5,    1,   2, 3.8,   4,  1.6],
  ["M2.5", 2.5, "xx", 0.45,   2,  1.1, 2.5, 4.5,   5,    2],
  ["M3",     3, "xx",  0.5, 2.5,  1.3,   3, 5.5, 5.5,  2.4],
  ["M4",     4, "xx",  0.7,   3,    2,   4,   7,   7,  3.2],
  ["M5",     5, "xx",  0.8,   4,  2.5,   5, 8.5,   8,  4.7],
  ["M6",     6, "xx",    1,   5,    3,   6,  10,  10,  5.2],
  ["M8",     8, "xx", 1.25,   6,    4,   8,  13,  13,  6.8],
  ["M10",   10, "xx",  1.5,   8,    5,  10,  16,  16,  8.4],
  ["M12",   12, "xx", 1.75,  10,    6,  12,  18,  18, 10.8],
  ["M16",   16, "xx",    2,  14,    8,  16,  24,  24, 14.8],
  ["M20",   20, "xx",  2.5,  17,   10,  20,  30,  30,   18],
];
