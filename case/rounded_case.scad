// =============================================================================
//  Rounded — case for the Waveshare ESP32-S3-Touch-AMOLED-1.75 lean-angle display
// =============================================================================
//  Shape: round display in the middle, rectangular "wings" left/right
//  (circle bulges out top and bottom).
//
//  Three printed parts + 2 small button plungers:
//    bezel  - front frame, holds the display module behind a lip.  Print face down.
//    tub    - its floor screws to the board's 3 M2 nuts and clamps the display
//             against the bezel lip; holds the LiPo and the GPS ceramic patch.
//    lid    - back cover, optional GoPro-style 2-prong mount.  Print inner face down.
//    plunger- push-rods for PWR / BOOT (insert from outside AFTER the display).
//
//  Hardware:
//    3x M2 x 5   (tub floor -> board's M2 nuts, H3.5)
//    4x M2.5 x 25 countersunk (lid -> tub pillars -> bezel bosses, self-tap in plastic)
//
//  Z = 0 is the front face. All parts are modelled in assembled position.
//  Units: mm.
// =============================================================================

/* [Output] */
part = "assembly"; // [assembly, exploded, bezel, tub, lid, plunger_pwr, plunger_boot]

/* [Board orientation] */
// Board rotation in the case (deg, CCW, seen from the front). Board-native: USB at 3 o'clock.
// 270 = USB at the bottom bulge, PWR/BOOT at the top bulge -> short direct access.
//       Needs the UI rotated 90 deg in firmware.
// 180 = matches current firmware (DISPLAY_ROTATE_180): USB goes out the LEFT wing
//       through a ~30 mm tunnel (needs a slim USB-C plug), buttons out the right wing.
board_rot = 270; // [180, 270]

/* [Layout] */
// X of the GPS patch's inner edge. 23.5 = fully beside the PCB (best reception, case ~109 mm wide).
// ~15.5 = compact (~93 mm wide), patch partly behind the PCB.
gps_x0 = 23.5;
// Height of the rectangular part
H = 44;
wall = 2.0;
fit = 0.3;            // clearance per side for pockets
corner_r = 3;         // outer corner rounding
fillet_r = 3;         // fillet where circle meets rectangle

/* [Display module (Waveshare drawing)] */
glass_d = 48.96;      // widest part of the cover glass
board_d = 46.0;
mod_h = 10.4;         // glass front to back of M2 nuts / connectors
pcb_back = 6.9;       // glass front to PCB back surface
skin = 1.2;           // front lip thickness
win_d = 45.5;         // visible window (glass front face is 44.16)

/* [Ports — verify with calipers on your board] */
usb_z_mod = 7.6;      // USB-C centre, measured from the glass front
usb_w = 13.0;
usb_h = 7.5;
btn_z_mod = 7.6;      // PWR/BOOT actuator centre, measured from the glass front
btn_hole = 3.1;
btn_shaft = 2.8;
btn_head_d = 4.6;
btn_cb = 1.0;         // counterbore depth for plunger head
btn_proud = 0.5;      // plunger travel (head stands this far above counterbore floor)
plunger_tip_r = 23.3; // radius where the plunger tip touches the button (board edge = 23)

/* [Battery / GPS antenna] */
bat = [55, 25, 13];
ant = [28, 28, 10];
cable_gap = 5;        // gap between battery and antenna (cable pass-through to the board)

/* [Back] */
floor_t = 1.6;
lid_t = 2.0;
gopro = true;

/* [Hidden] */
$fn = 96;
eps = 0.01;

// ---------------------------------------------------------------- derived ----
bore_r = glass_d/2 + fit;         // display bore
R_out  = bore_r + 2.2;            // outer circle
ring_r = bore_r + 1.6;            // ring wall between display bore and wing hollows
bat_c  = [bat.x + 2*fit, bat.y + 2*fit, bat.z];
ant_c  = [ant.x + 2*fit, ant.y + 2*fit, ant.z];
W      = 2*(gps_x0 + ant_c.x + 0.5 + wall);
bez_h  = skin + mod_h;            // tub floor touches the M2 nuts here
cav    = bat.z + 1.6;             // battery may rest on the M2 screw heads
tub_h  = floor_t + cav;
z_usb  = skin + usb_z_mod;
z_btn  = skin + btn_z_mod;

bat_x0 = gps_x0 - cable_gap - bat_c.x;   // battery pocket left edge
pil_d  = 6.4;
pil_pos = [for (sx = [-1, 1], sy = [-1, 1]) [sx*(W/2 - 5), sy*(H/2 - 4.2)]];

function rot2(p, a) = [p.x*cos(a) - p.y*sin(a), p.x*sin(a) + p.y*cos(a)];
mnt_b  = [[0, 20.5], [13.75, -14.7], [-13.75, -14.7]];   // board M2 nuts (native)
mnt    = [for (p = mnt_b) rot2(p, board_rot)];
usb_a  = board_rot;                      // USB at 0 deg native
btn_a  = [156 + board_rot, 206 + board_rot]; // PWR, BOOT native angles

// distance from centre to the outside along angle a (circle U rectangle)
function r_exit(a) = max(R_out, min(
    abs(cos(a)) > 1e-6 ? (W/2)/abs(cos(a)) : 1e9,
    abs(sin(a)) > 1e-6 ? (H/2)/abs(sin(a)) : 1e9));

assert(bat_x0 - 1.2 >= -(W/2 - wall), "battery does not fit: raise gps_x0");
assert(H/2 - wall >= ant_c.y/2 + 0.2, "antenna does not fit: raise H");
assert(z_btn + (btn_head_d + 0.4)/2 <= bez_h - 0.3, "button counterbore hits the bezel rim: lower btn_z_mod or btn_head_d");

// ------------------------------------------------------------- 2D helpers ----
module outline(d = 0) {
    offset(delta = d)
    offset(r = corner_r) offset(delta = -corner_r)        // round convex corners
    offset(r = -fillet_r) offset(delta = fillet_r)        // fillet concave corners
        union() { circle(r = R_out); square([W, H], center = true); }
}

// prism with stepped chamfers (cb at the bottom, ct at the top)
module cprism(h, cb = 0, ct = 0, d = 0, n = 4) {
    translate([0, 0, cb]) linear_extrude(h - cb - ct) outline(d);
    if (cb > 0) for (i = [0:n-1])
        translate([0, 0, cb*i/n]) linear_extrude(cb/n + eps) outline(d - cb*(n - i - 0.5)/n);
    if (ct > 0) for (i = [0:n-1])
        translate([0, 0, h - ct + ct*i/n]) linear_extrude(ct/n + eps) outline(d - ct*(i + 0.5)/n);
}

// radial box / cylinder along angle a
module rbox(a, r0, r1, w, z0, z1)
    rotate([0, 0, a]) translate([r0, -w/2, z0]) cube([r1 - r0, w, z1 - z0]);
module rcyl(a, r0, r1, d, z)
    rotate([0, 0, a]) translate([r0, 0, z]) rotate([0, 90, 0]) cylinder(d = d, h = r1 - r0);

// ------------------------------------------------------------------ ports ----
module port_holes() {
    rbox(usb_a, bore_r - 2, r_exit(usb_a) + 2, usb_w, z_usb - usb_h/2, z_usb + usb_h/2);
    for (a = btn_a) {
        rcyl(a, bore_r - 1, r_exit(a) + 2, btn_hole, z_btn);
        rcyl(a, r_exit(a) - btn_cb, r_exit(a) + 3, btn_head_d + 0.4, z_btn);
    }
}

// solid material around the port channels (matters when they cross a hollow wing)
module port_ducts() {
    intersection() {
        linear_extrude(bez_h) outline();
        union() {
            rbox(usb_a, bore_r, r_exit(usb_a) + 2, usb_w + 3.2, z_usb - usb_h/2 - 1.6, bez_h);
            for (a = btn_a) rcyl(a, bore_r, r_exit(a) + 2, btn_head_d + 3.4, z_btn);
        }
    }
}

// ------------------------------------------------------------------ bezel ----
module bezel() {
    difference() {
        union() {
            difference() {
                cprism(bez_h, cb = 0.8);
                translate([0, 0, skin]) cylinder(r = bore_r, h = bez_h);              // display bore
                translate([0, 0, -1]) cylinder(d = win_d, h = skin + 2);              // window
                translate([0, 0, -eps]) cylinder(d1 = win_d + 1.6, d2 = win_d, h = 0.8); // window chamfer
                translate([0, 0, skin])                                               // wing hollows
                    linear_extrude(bez_h) difference() { outline(-wall); circle(r = ring_r); }
            }
            for (p = pil_pos) translate([p.x, p.y, 0]) cylinder(d = pil_d, h = bez_h); // screw bosses
            port_ducts();
        }
        for (p = pil_pos) translate([p.x, p.y, skin + 0.6]) cylinder(d = 2.2, h = bez_h); // M2.5 pilot
        port_holes();
    }
}

// -------------------------------------------------------------------- tub ----
module frame2d(x, y, t = 1.2)
    difference() { translate([-t, -t]) square([x + 2*t, y + 2*t]); square([x, y]); }

module ribs2d() {
    difference() {
        union() {
            translate([bat_x0, -bat_c.y/2]) frame2d(bat_c.x, bat_c.y);
            translate([gps_x0, -ant_c.y/2]) frame2d(ant_c.x, ant_c.y);
        }
        translate([gps_x0 - cable_gap - 2, -bat_c.y/2 + 3]) square([cable_gap + 4, bat_c.y - 6]);
        for (p = mnt) translate(p) circle(d = 5.5);
    }
}

module cable_slot2d()   // battery + GPS cables down to the board
    translate([gps_x0 - cable_gap - 0.5, 2.5]) square([cable_gap, 7]);

module tub() {
    difference() {
        translate([0, 0, bez_h]) difference() {
            union() {
                difference() {
                    linear_extrude(tub_h) outline();
                    translate([0, 0, floor_t]) linear_extrude(tub_h) outline(-wall);
                }
                for (p = pil_pos) translate([p.x, p.y, 0]) cylinder(d = pil_d, h = tub_h);
                translate([0, 0, floor_t - eps]) linear_extrude(3) ribs2d();
            }
            for (p = pil_pos) translate([p.x, p.y, -1]) cylinder(d = 2.9, h = tub_h + 2);
            for (p = mnt) translate([p.x, p.y, -1]) cylinder(d = 2.4, h = floor_t + 2);
            translate([0, 0, -1]) linear_extrude(floor_t + 2) cable_slot2d();
        }
        port_holes();
    }
}

// -------------------------------------------------------------------- lid ----
module gopro_2prong(t = 3.0, gap = 3.3, hole = 5.3, r = 7.5, hgt = 9) {
    for (x0 = [-gap/2 - t, gap/2])
        translate([x0, 0, 0]) rotate([90, 0, 90]) linear_extrude(t)
            difference() {
                hull() { translate([-r, 0]) square([2*r, 1]); translate([0, hgt]) circle(r = r); }
                translate([0, hgt]) circle(d = hole);
            }
}

module lid() {
    translate([0, 0, bez_h + tub_h]) difference() {
        union() {
            cprism(lid_t, ct = 0.6);
            if (gopro) translate([0, 0, lid_t - eps]) gopro_2prong();
        }
        for (p = pil_pos) translate([p.x, p.y, 0]) {
            translate([0, 0, -1]) cylinder(d = 2.9, h = lid_t + 2);
            translate([0, 0, lid_t - 1.4]) cylinder(d1 = 2.9, d2 = 5.7, h = 1.4 + eps); // countersink
        }
    }
}

// ---------------------------------------------------------------- plunger ----
// printed head-down; head sits in the counterbore, tip touches the button
module plunger(a) {
    head_in = r_exit(a) - btn_cb + btn_proud;   // radius of the head's inner face
    L    = head_in - plunger_tip_r;
    snap = head_in - (bore_r - 0.35);           // snap ring just inside the wall
    cylinder(d = btn_head_d, h = btn_cb);
    translate([0, 0, btn_cb - eps]) cylinder(d = btn_shaft, h = L);
    if (snap < L - 0.3)
        translate([0, 0, btn_cb + snap]) cylinder(d1 = btn_hole + 0.25, d2 = btn_shaft, h = 0.5);
}

// ---------------------------------------------------------- dummy parts -----
module dummies() {
    color("black") translate([0, 0, skin]) cylinder(d = glass_d, h = 2.05);
    color("royalblue") translate([0, 0, skin + 2.05]) cylinder(d = board_d, h = pcb_back - 2.05);
    color("gold") for (p = mnt) translate([p.x, p.y, skin + pcb_back]) cylinder(d = 3.5, h = mod_h - pcb_back);
    color("silver") rbox(usb_a, 19, 24.4, 9, z_usb - 1.6, z_usb + 1.6);
    color("dimgray") translate([bat_x0 + fit, -bat.y/2, bez_h + floor_t + 1.4]) cube(bat);
    color("tan") translate([gps_x0 + fit, -ant.y/2, bez_h + floor_t]) cube(ant);
}

module plunger_placed(a)
    rotate([0, 0, a]) translate([r_exit(a) - btn_cb + btn_proud + btn_cb, 0, z_btn])
        rotate([0, -90, 0]) plunger(a);

// --------------------------------------------------------------- output -----
if (part == "assembly") {
    color("dimgray", 0.9) bezel();
    color("slategray", 0.6) tub();
    color("darkgray", 0.9) lid();
    color("orange") for (a = btn_a) plunger_placed(a);
    dummies();
} else if (part == "exploded") {
    color("dimgray") bezel();
    translate([0, 0, 25]) color("slategray") tub();
    translate([0, 0, 50]) color("darkgray") lid();
    color("orange") for (a = btn_a) plunger_placed(a);
    translate([0, 0, 12]) dummies();
} else if (part == "bezel") {
    bezel();                                                // face down, as modelled
} else if (part == "tub") {
    translate([0, 0, -bez_h]) tub();                        // floor down
} else if (part == "lid") {
    translate([0, 0, -(bez_h + tub_h)]) lid();              // inner face down, prongs up
} else if (part == "plunger_pwr") {
    plunger(btn_a[0]);
} else if (part == "plunger_boot") {
    plunger(btn_a[1]);
}

echo(str("Case outline: ", W, " x ", 2*R_out, " mm (rect height ", H,
         "), thickness ", bez_h + tub_h + lid_t, " mm (+9+7.5 mm GoPro prongs)"));
