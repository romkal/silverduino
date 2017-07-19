PARTNO = undef;

a_l = 68.6;
a_w = 53.3;
a_h = 40;
pad=3;
usb=[[9.6, 1.6], [12, 10.9]];
charge=[[a_w - 3.3 - 8.9, 1.6], [8.9, 10.9]];
din=[[22, 20], [12, 12]];
side=7;
w = a_w + side * 2;
l = 70;
t = 1;
d = 0.5;
edge = [-w / 2, -l / 2, 0];
module cover(h) {
    translate(edge + [-t, -t, h]) cube([w + t * 2, l + t * 2, t]);
}

module wall() {
    difference() {
        translate(edge - [t, t, 0]) cube([w + 2*t, l + 2*t, a_h]);
        translate(edge) cube([w, l, a_h]);
    }
}

module screws() {
    l = a_h / 2;
    module screw() {
        translate(edge + [side / 3, side / 3, a_h - l]) {
            cylinder(l, r=1.5);
            translate([0, 0, l]) cylinder(t, r1=1.5, r2=2.5);
        }
    }
    screw();
    mirror([0, 1, 0]) screw();
    mirror([1, 0, 0]) screw();
    mirror([1, 0, 0]) mirror([0, 1, 0]) screw();
}

module supports() {
    module support() {
        translate(edge) {
            linear_extrude(height=a_h) polygon([[0, 0], [side, 0], [0, side]]);
        }
    }
    support();
    mirror([0, 1, 0]) support();
    mirror([1, 0, 0]) support();
    mirror([1, 0, 0]) mirror([0, 1, 0]) support();
}

module cut(size) {
    rotate([90, 0, 0]) minkowski() {
        cylinder(1, $fn=10, r=d, center=true);
        cube([size[0], size[1], 10]);
    }
}

module ports(extra_height = 0) {
    module port(p) {
        translate([p[0][0],5 ,p[0][1]]) cut([p[1][0], p[1][1] + extra_height]);
    }
    translate([-a_w / 2, -l / 2, pad]) {
        port(usb);
        port(charge);
        port(din);
    }
}
module base() {
    module hulls() {
        for(i = [0.2:0.1:0.7]) {
            hull() {
                translate(edge + [0, 0, 0]) {
                    translate([w * i, l * 0.2, 0]) cylinder(5, r=1, center = true);
                    translate([w * 0.2, l * i, 0]) cylinder(5, r=1, center = true);

                }
            }
        }
    }
    difference() {
        cover(-1);
        hulls();
        mirror([1, 1, 0]) hulls();
    }
    difference() {
        supports();
        screws();
    }
    difference() {
        wall();
        ports(a_h);
    }

    hx = [2.5, 5.1, 27.9, 15.2];
    hy = [14, 1.3, 50.8, 0];
    translate([-a_w / 2, -l / 2, 0]) {
        pad([a_w - hx[0], hy[0], 0]);
        pad([a_w - hx[0] - hx[1], hy[0] + hy[1] + hy[2], 0]);
        pad([a_w - hx[0] - hx[1] - hx[2], hy[0] + hy[1] + hy[2], 0]);
        pad([a_w - hx[0] - hx[1] - hx[2] - hx[3], hy[0] + hy[1], 0]);
    }
    module pad(pos) {
        translate(pos) {
            cylinder(pad, r=2.5);
            cylinder(r=1.5, h=pad + 3);
        }
    }
}

module screen_port() {
    module drill() translate([15, 7, 0]) cylinder(5, r=2, $fn=10, center = true);
    translate([0, -l / 4, a_h]) {
        cube([25, 20, 5], center = true);
        drill();
        mirror([1, 0, 0]) drill();
        mirror([0, 1, 0]) drill();
        mirror([1, 0, 0]) mirror([0, 1, 0]) drill();
    }
}

module top() {
    difference() {
        union() {
            cover(a_h);
            wall();
            translate(edge + [3, l - 5, a_h + t]) rotate([0, 0, -90]) linear_extrude(height=1) text(size=7, font="Purisa", "Silverduino");
        }
        base();
        ports();
        screen_port();
        hull() dial();
        screws();
    }

}

module dial() {
    d_h = 10;
    translate([0, l / 4, a_h - d_h / 2]) {
        difference() {
            cylinder(d_h + t, r=l / 5 + d(3));
            cylinder(d_h / 2, $fn=20, r = 3 + d);
            translate([0, l / 10, d_h + 9.9]) sphere(r=10);
        }
        translate([1.5 + d, -3, 0]) cube([6, 6, d_h]);
    }
}

function d(part) = part == PARTNO ? 0 : d;
if (PARTNO == 1)
    base();
if (PARTNO == 2)
    top();
if (PARTNO == 3)
    dial();
if (PARTNO == undef) {
    base();
    top();
    dial();
}


