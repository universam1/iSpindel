$fn=128;

l=118;    // total length
pl=44;    // boasrd length

co=15;    // cut offset

nw=6;     // nut width
nd=2.7;   // nut slot wifth
nso=-1.5; // nut slot offset

h=3.5;    // mounting hole diameter
ho=10;    // mounting hole offset

difference() {
  union() {

    difference() {
      union() {
        difference() {
          translate([-17.78,0,-5]) cube([4.6,l,10]);
          translate([-15.75,-1,1.15]) cube([2.65,47,1.9]);
        }
        difference() {
          translate([17.78-4.6,0,-5]) cube([4.6,l,10]);
          translate([15.75-2.65,-1,1.15]) cube([2.65,47,1.9]);
        }
        translate([-17.78,pl,-5]) cube([17.78*2,l-pl,10]);
      }


      translate([14,l,0]) 
        rotate([0,0,25]) 
          translate([0,-20,-10]) cube([40,40,20]);
      
      translate([-14,l,0]) 
        rotate([0,0,-25]) 
          translate([-40,-20,-10]) cube([40,40,20]);

      rotate([-3,0,0])
        translate([0,65+pl+2,12]) 
          rotate([90,0,0]) 
            cylinder(65.2,18.5/2,18.5/2);

      translate([-12.8,pl+3+(65.2/2),-6]) cube([2,4,12]);
      translate([12.8-2,pl+3+(65.2/2),-6]) cube([2,4,12]);

      translate([ho,l+0.01,0]) rotate([90,0,0]) cylinder(8,h/2,h/2);
      translate([-ho,l+0.01,0]) rotate([90,0,0]) cylinder(8,h/2,h/2);

      translate([-ho,l+nso,0]) rotate([90,30,0]) nut(nw,nd);
      translate([-nw/2-ho,l-nd+nso,0]) cube([nw,nd,10]);

      translate([ho,l+nso,0]) rotate([90,30,0]) nut(nw,nd);
      translate([-nw/2+ho,l-nd+nso,0]) cube([nw,nd,10]);

    }
  }
}

module nut(d,h) { 
  translate([0,0,h/2]) {
    cube(size = [d/1.75,d,h], center = true);
    rotate([0,0,60]) cube(size = [d/1.75,d,h], center = true);
    rotate([0,0,-60]) cube(size = [d/1.75,d,h], center = true);
  }
}
