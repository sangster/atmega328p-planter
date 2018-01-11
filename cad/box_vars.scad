box_w         = 120;
box_h         =  90;
box_thickness =   2.5;
box_corner_radius =   5;

lid_shell_h = 4.2;
lid_lip_w = 1.8;
lid_lip_h = 3.8;

lid_hole_od = 5;
lid_hole_id = 3;
lid_hole_space = 0.2;

col_od        =   8;
col_id        =   2.5;
col_h         = box_h - 2;
wall_space    =   1;
col_notch_h   =   0.2;
col_notch_d   =   5.5;

bar_w         =   3;
bar_h         = box_h - lid_lip_h;
bar_space     =   2;
bar_count     =  19;
fence_width   = (bar_w + bar_space) * bar_count - bar_space;

notch_w       = bar_w / 2;
notch_h       = box_thickness + 2.5;
notch_len     = wall_space + col_od / 2;
