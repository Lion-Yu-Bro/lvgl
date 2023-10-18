/**
 * @file lv_demo_benchmark.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_benchmark.h"

#if LV_USE_DEMO_BENCHMARK
#include "../../src/display/lv_display_private.h"
#include "../../src/core/lv_global.h"

/**********************
 *      TYPEDEFS
 **********************/

#define SCENE_TIME  5000


typedef struct {
    const char * name;
    void (*create_cb)(void);
} scene_dsc_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void load_scene(uint32_t scene);
static void next_scene_timer_cb(lv_timer_t * timer);
static void sysmon_perf_observer_cb(lv_subject_t * subject, lv_observer_t * observer);
static int32_t rnd_next(int32_t min, int32_t max);
static void shake_anim_y_cb(void * var, int32_t v);
static void shake_anim(lv_obj_t * obj, lv_coord_t y_max);
static void scroll_anim(lv_obj_t * obj, lv_coord_t y_max);
static void scroll_anim_y_cb(void * var, int32_t v);
static void color_anim_cb(void * var, int32_t v);
static void color_anim(lv_obj_t * obj);
static void arc_anim(lv_obj_t * obj);

static lv_obj_t * card_create(void);

static void empty_screen_cb(void)
{
    color_anim(lv_screen_active());
}

static void moving_wallpaper_cb(void)
{
    lv_obj_set_style_pad_all(lv_screen_active(), 0, 0);
    LV_IMG_DECLARE(img_benchmark_cogwheel_rgb);

    lv_obj_t * img = lv_img_create(lv_screen_active());
    lv_obj_set_size(img, lv_pct(150), lv_pct(150));
    lv_img_set_src(img, &img_benchmark_cogwheel_rgb);
    shake_anim(img, - lv_display_get_vertical_resolution(NULL) / 3);
}

static void single_rectangle_cb(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_remove_style_all(obj);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_center(obj);
    lv_obj_set_size(obj, lv_pct(30), lv_pct(30));

    color_anim(obj);

}

static void multiple_rectangles_cb(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_SPACE_EVENLY);

    uint32_t i;
    for(i = 0; i < 9; i++) {
        lv_obj_t * obj = lv_obj_create(lv_screen_active());
        lv_obj_remove_style_all(obj);
        lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
        lv_obj_set_size(obj, lv_pct(25), lv_pct(25));

        color_anim(obj);
    }
}

static void multiple_rgb_images_cb(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(lv_screen_active(), 20, 0);

    LV_IMG_DECLARE(img_benchmark_cogwheel_rgb);
    uint32_t hor_cnt = (lv_display_get_horizontal_resolution(NULL) - 16) / 116;
    uint32_t ver_cnt = (lv_display_get_vertical_resolution(NULL) - 116) / 116;

    uint32_t y;
    for(y = 0; y < ver_cnt; y++) {
        uint32_t x;
        for(x = 0; x < hor_cnt; x++) {
            lv_obj_t * obj = lv_img_create(lv_screen_active());
            lv_image_set_src(obj, &img_benchmark_cogwheel_rgb);
            if(x == 0) lv_obj_add_flag(obj, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);

            shake_anim(obj, 80);
        }
    }
}

static void multiple_argb_images_cb(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(lv_screen_active(), 20, 0);

    LV_IMG_DECLARE(img_benchmark_cogwheel_argb);
    uint32_t hor_cnt = (lv_display_get_horizontal_resolution(NULL) - 16) / 116;
    uint32_t ver_cnt = (lv_display_get_vertical_resolution(NULL) - 116) / 116;

    uint32_t y;
    for(y = 0; y < ver_cnt; y++) {
        uint32_t x;
        for(x = 0; x < hor_cnt; x++) {
            lv_obj_t * obj = lv_img_create(lv_screen_active());
            lv_image_set_src(obj, &img_benchmark_cogwheel_argb);
            if(x == 0) lv_obj_add_flag(obj, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);

            shake_anim(obj, 80);
        }
    }
}

static void rotated_argb_image_cb(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(lv_screen_active(), 20, 0);


    LV_IMG_DECLARE(img_benchmark_cogwheel_argb);
    uint32_t hor_cnt = (lv_display_get_horizontal_resolution(NULL) - 16) / 116;
    uint32_t ver_cnt = (lv_display_get_vertical_resolution(NULL) - 116) / 116;

    uint32_t y;
    for(y = 0; y < ver_cnt; y++) {
        uint32_t x;
        for(x = 0; x < hor_cnt; x++) {
            lv_obj_t * obj = lv_img_create(lv_screen_active());
            lv_image_set_src(obj, &img_benchmark_cogwheel_argb);
            if(x == 0) lv_obj_add_flag(obj, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);

            lv_image_set_rotation(obj, lv_rand(100, 3500));
            shake_anim(obj, 80);
        }
    }
}

static void multiple_labels_cb(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(lv_screen_active(), 80, 0);

    lv_point_t s;
    lv_text_get_size(&s, "Hello LVGL!", lv_obj_get_style_text_font(lv_screen_active(), 0), 0, 0, LV_COORD_MAX,
                     LV_TEXT_FLAG_NONE);

    uint32_t cnt = (lv_display_get_horizontal_resolution(NULL) - 16) / (s.x + 30);
    cnt = cnt * ((lv_display_get_vertical_resolution(NULL) - 200) / (s.y + 50));

    uint32_t i;
    for(i = 0; i < cnt; i++) {
        lv_obj_t * obj = lv_label_create(lv_screen_active());
        lv_label_set_text(obj, "Hello LVGL!");
        color_anim(obj);
    }
}

static void screen_sized_text_cb(void)
{
    lv_obj_t * obj = lv_label_create(lv_screen_active());
    lv_obj_set_width(obj, lv_pct(100));

    if(lv_display_get_horizontal_resolution(NULL) * lv_display_get_vertical_resolution(NULL) < 150000) {
        lv_label_set_text(obj,
                          "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nulla nec rhoncus arcu, in consectetur orci. Sed vitae dolor sed nisi ultrices vehicula quis ac dolor. Vivamus hendrerit hendrerit lectus, sed tempus velit suscipit in. Fusce eu tristique arcu. Sed et molestie leo, in lacinia nunc. Quisque semper lorem sed ante feugiat, at molestie risus blandit. Maecenas lobortis urna in diam feugiat porta. Ut facilisis mauris eget nibh posuere aliquet. Proin facilisis egestas magna, id vulputate massa bibendum a. Etiam gravida metus non egestas suscipit. Sed sollicitudin mollis nisi, eu fringilla leo vestibulum posuere. Donec et ex nulla. Phasellus et ornare justo, vel hendrerit justo. Curabitur pulvinar nunc sed tincidunt dignissim. Praesent eleifend lectus velit, id malesuada ante placerat id. Fusce massa erat, egestas vel venenatis eu, tempus nec est.\n\n"
                          "Phasellus iaculis malesuada molestie. Cras ullamcorper justo a dolor dignissim tincidunt. Mauris euismod risus quis lobortis mollis. Ut vitae placerat massa, aliquet varius lectus. Nulla ac ornare purus, quis auctor velit. Donec posuere dolor rhoncus efficitur dictum. Integer venenatis aliquet nunc eu convallis. Nunc quis varius velit. Suspendisse enim metus, molestie eget mauris sit amet, euismod volutpat turpis. Duis rhoncus commodo gravida. Pellentesque velit mi, dictum id consequat placerat, condimentum ac elit. Duis aliquet leo eu dolor cursus rhoncus. Quisque aliquam sapien ut purus hendrerit laoreet. Ut venenatis venenatis risus, a vestibulum enim lobortis a. Maecenas auctor tortor lorem, quis laoreet nulla aliquet a. Sed ipsum lorem, facilisis in congue a, dictum ut ligula.\n\n"
                          "Aliquam id tellus in enim hendrerit mattis. Sed ipsum arcu, feugiat sed eros quis, vulputate facilisis turpis. Quisque venenatis risus massa. Proin lacinia, nunc non ultrices commodo, ligula dolor lobortis lectus, iaculis pulvinar metus orci eu elit. Donec tincidunt lacinia semper. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos. Donec vitae odio risus. Donec sodales sed nulla sit amet iaculis. Duis lacinia mauris dictum, fermentum nibh eget, convallis tellus. Sed congue luctus purus non scelerisque. Etiam fermentum lacus mauris, at bibendum nunc aliquam at. Vivamus accumsan vestibulum pharetra. Proin rhoncus nisi purus, vel blandit metus auctor eget. Fusce dictum sed lectus sed aliquam. Praesent lobortis quam sed pretium tincidunt.");
    }
    else {
        lv_label_set_text(obj,
                          "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nulla nec rhoncus arcu, in consectetur orci. Sed vitae dolor sed nisi ultrices vehicula quis ac dolor. Vivamus hendrerit hendrerit lectus, sed tempus velit suscipit in. Fusce eu tristique arcu. Sed et molestie leo, in lacinia nunc. Quisque semper lorem sed ante feugiat, at molestie risus blandit. Maecenas lobortis urna in diam feugiat porta. Ut facilisis mauris eget nibh posuere aliquet. Proin facilisis egestas magna, id vulputate massa bibendum a. Etiam gravida metus non egestas suscipit. Sed sollicitudin mollis nisi, eu fringilla leo vestibulum posuere. Donec et ex nulla. Phasellus et ornare justo, vel hendrerit justo. Curabitur pulvinar nunc sed tincidunt dignissim. Praesent eleifend lectus velit, id malesuada ante placerat id. Fusce massa erat, egestas vel venenatis eu, tempus nec est.\n\n"
                          "Phasellus iaculis malesuada molestie. Cras ullamcorper justo a dolor dignissim tincidunt. Mauris euismod risus quis lobortis mollis. Ut vitae placerat massa, aliquet varius lectus. Nulla ac ornare purus, quis auctor velit. Donec posuere dolor rhoncus efficitur dictum. Integer venenatis aliquet nunc eu convallis. Nunc quis varius velit. Suspendisse enim metus, molestie eget mauris sit amet, euismod volutpat turpis. Duis rhoncus commodo gravida. Pellentesque velit mi, dictum id consequat placerat, condimentum ac elit. Duis aliquet leo eu dolor cursus rhoncus. Quisque aliquam sapien ut purus hendrerit laoreet. Ut venenatis venenatis risus, a vestibulum enim lobortis a. Maecenas auctor tortor lorem, quis laoreet nulla aliquet a. Sed ipsum lorem, facilisis in congue a, dictum ut ligula.\n\n"
                          "Aliquam id tellus in enim hendrerit mattis. Sed ipsum arcu, feugiat sed eros quis, vulputate facilisis turpis. Quisque venenatis risus massa. Proin lacinia, nunc non ultrices commodo, ligula dolor lobortis lectus, iaculis pulvinar metus orci eu elit. Donec tincidunt lacinia semper. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos. Donec vitae odio risus. Donec sodales sed nulla sit amet iaculis. Duis lacinia mauris dictum, fermentum nibh eget, convallis tellus. Sed congue luctus purus non scelerisque. Etiam fermentum lacus mauris, at bibendum nunc aliquam at. Vivamus accumsan vestibulum pharetra. Proin rhoncus nisi purus, vel blandit metus auctor eget. Fusce dictum sed lectus sed aliquam. Praesent lobortis quam sed pretium tincidunt.\n\n"
                          "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nulla nec rhoncus arcu, in consectetur orci. Sed vitae dolor sed nisi ultrices vehicula quis ac dolor. Vivamus hendrerit hendrerit lectus, sed tempus velit suscipit in. Fusce eu tristique arcu. Sed et molestie leo, in lacinia nunc. Quisque semper lorem sed ante feugiat, at molestie risus blandit. Maecenas lobortis urna in diam feugiat porta. Ut facilisis mauris eget nibh posuere aliquet. Proin facilisis egestas magna, id vulputate massa bibendum a. Etiam gravida metus non egestas suscipit. Sed sollicitudin mollis nisi, eu fringilla leo vestibulum posuere. Donec et ex nulla. Phasellus et ornare justo, vel hendrerit justo. Curabitur pulvinar nunc sed tincidunt dignissim. Praesent eleifend lectus velit, id malesuada ante placerat id. Fusce massa erat, egestas vel venenatis eu, tempus nec est.\n\n"
                          "Phasellus iaculis malesuada molestie. Cras ullamcorper justo a dolor dignissim tincidunt. Mauris euismod risus quis lobortis mollis. Ut vitae placerat massa, aliquet varius lectus. Nulla ac ornare purus, quis auctor velit. Donec posuere dolor rhoncus efficitur dictum. Integer venenatis aliquet nunc eu convallis. Nunc quis varius velit. Suspendisse enim metus, molestie eget mauris sit amet, euismod volutpat turpis. Duis rhoncus commodo gravida. Pellentesque velit mi, dictum id consequat placerat, condimentum ac elit. Duis aliquet leo eu dolor cursus rhoncus. Quisque aliquam sapien ut purus hendrerit laoreet. Ut venenatis venenatis risus, a vestibulum enim lobortis a. Maecenas auctor tortor lorem, quis laoreet nulla aliquet a. Sed ipsum lorem, facilisis in congue a, dictum ut ligula.\n\n"
                          "Aliquam id tellus in enim hendrerit mattis. Sed ipsum arcu, feugiat sed eros quis, vulputate facilisis turpis. Quisque venenatis risus massa. Proin lacinia, nunc non ultrices commodo, ligula dolor lobortis lectus, iaculis pulvinar metus orci eu elit. Donec tincidunt lacinia semper. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos. Donec vitae odio risus. Donec sodales sed nulla sit amet iaculis. Duis lacinia mauris dictum, fermentum nibh eget, convallis tellus. Sed congue luctus purus non scelerisque. Etiam fermentum lacus mauris, at bibendum nunc aliquam at. Vivamus accumsan vestibulum pharetra. Proin rhoncus nisi purus, vel blandit metus auctor eget. Fusce dictum sed lectus sed aliquam. Praesent lobortis quam sed pretium tincidunt.");
    }

    scroll_anim(lv_screen_active(), LV_VER_RES - lv_obj_get_height(obj));
}

static void multiple_arcs_cb(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_SPACE_EVENLY);

    LV_IMG_DECLARE(img_benchmark_cogwheel_argb);
    uint32_t hor_cnt = (lv_display_get_horizontal_resolution(NULL) - 16) / lv_dpx(160);
    uint32_t ver_cnt = (lv_display_get_vertical_resolution(NULL) - 16) / lv_dpx(160);

    uint32_t y;
    for(y = 0; y < ver_cnt; y++) {
        uint32_t x;
        for(x = 0; x < hor_cnt; x++) {

            lv_obj_t * obj = lv_arc_create(lv_screen_active());
            if(x == 0) lv_obj_add_flag(obj, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
            lv_obj_set_size(obj, lv_dpx(100), lv_dpx(100));
            lv_obj_center(obj);

            lv_arc_set_bg_angles(obj, 0, 360);

            lv_obj_set_style_margin_all(obj, lv_dpx(20), 0);
            lv_obj_set_style_arc_opa(obj, 0, LV_PART_MAIN);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_KNOB);
            lv_obj_set_style_arc_width(obj, 10, LV_PART_INDICATOR);
            lv_obj_set_style_arc_rounded(obj, false, LV_PART_INDICATOR);
            lv_obj_set_style_arc_color(obj, lv_color_hex3(lv_rand(0x00f, 0xff0)), LV_PART_INDICATOR);
            arc_anim(obj);
        }
    }
}


static void containers_cb(void)
{

    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_SPACE_EVENLY);

    uint32_t hor_cnt = (lv_display_get_horizontal_resolution(NULL) - 16) / 300;
    uint32_t ver_cnt = (lv_display_get_vertical_resolution(NULL) - 16) / 150;

    uint32_t y;
    for(y = 0; y < ver_cnt; y++) {
        uint32_t x;
        for(x = 0; x < hor_cnt; x++) {
            lv_obj_t * card = card_create();
            if(x == 0) lv_obj_add_flag(card, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
            shake_anim(card, 50);
        }
    }
}

static void containers_with_overlay_cb(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_SPACE_EVENLY);

    uint32_t hor_cnt = (lv_display_get_horizontal_resolution(NULL) - 16) / 300;
    uint32_t ver_cnt = (lv_display_get_vertical_resolution(NULL) - 16) / 150;

    uint32_t y;
    for(y = 0; y < ver_cnt; y++) {
        uint32_t x;
        for(x = 0; x < hor_cnt; x++) {
            lv_obj_t * card = card_create();
            if(x == 0) lv_obj_add_flag(card, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
            shake_anim(card, 50);
        }
    }

    lv_obj_set_style_bg_opa(lv_layer_top(), LV_OPA_50, 0);
    color_anim(lv_layer_top());
}

static void containers_with_opa_cb(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_SPACE_EVENLY);

    uint32_t hor_cnt = (lv_display_get_horizontal_resolution(NULL) - 16) / 300;
    uint32_t ver_cnt = (lv_display_get_vertical_resolution(NULL) - 16) / 150;

    uint32_t y;
    for(y = 0; y < ver_cnt; y++) {
        uint32_t x;
        for(x = 0; x < hor_cnt; x++) {
            lv_obj_t * card = card_create();
            if(x == 0) lv_obj_add_flag(card, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
            lv_obj_set_style_opa(card, LV_OPA_50, 0);
            shake_anim(card, 50);
        }
    }
}

static void containers_with_opa_layer_cb(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_SPACE_EVENLY);

    uint32_t hor_cnt = (lv_display_get_horizontal_resolution(NULL) - 16) / 300;
    uint32_t ver_cnt = (lv_display_get_vertical_resolution(NULL) - 16) / 150;

    uint32_t y;
    for(y = 0; y < ver_cnt; y++) {
        uint32_t x;
        for(x = 0; x < hor_cnt; x++) {
            lv_obj_t * card = card_create();
            lv_obj_set_style_opa_layered(card, LV_OPA_50, 0);
            if(x == 0) lv_obj_add_flag(card, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
            shake_anim(card, 50);
        }
    }
}

static void containers_with_scrolling_cb(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_pos(obj, rnd_next(0, 300), rnd_next(0, 300));
}

/**********************
 *  STATIC VARIABLES
 **********************/

static scene_dsc_t scenes[] = {
    {.name = "Empty screen",               .create_cb = empty_screen_cb},
    {.name = "Moving wallpaper",           .create_cb = moving_wallpaper_cb},
    {.name = "Single rectangle",           .create_cb = single_rectangle_cb},
    {.name = "Multiple rectangles",        .create_cb = multiple_rectangles_cb},
    {.name = "Multiple RGB images",        .create_cb = multiple_rgb_images_cb},
    {.name = "Multiple ARGB images",       .create_cb = multiple_argb_images_cb},
    {.name = "Rotated ARGB images",        .create_cb = rotated_argb_image_cb},
    {.name = "Multiple labels",            .create_cb = multiple_labels_cb},
    {.name = "Screen sized text",          .create_cb = screen_sized_text_cb},
    {.name = "Multiple arcs",              .create_cb = multiple_arcs_cb},

    {.name = "Containers",                 .create_cb = containers_cb},
    {.name = "Containers with overlay",    .create_cb = containers_with_overlay_cb},
    {.name = "Containers with opa",        .create_cb = containers_with_opa_cb},
    {.name = "Containers with opa_layer",  .create_cb = containers_with_opa_layer_cb},
    {.name = "Containers with scrolling",  .create_cb = containers_with_scrolling_cb},

    {.name = "", .create_cb = NULL}
};

#define sysmon_perf LV_GLOBAL_DEFAULT()->sysmon_perf

static uint32_t scene_act;
static uint32_t rnd_act;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_benchmark(void)
{
    scene_act = 0;

    lv_obj_t * scr = lv_screen_active();
    lv_obj_remove_style_all(scr);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(scr, lv_color_black(), 0);
    lv_obj_set_style_bg_color(scr, lv_palette_lighten(LV_PALETTE_GREY, 4), 0);
    lv_obj_set_style_pad_all(lv_screen_active(), 8, 0);
    lv_obj_set_style_pad_top(lv_screen_active(), 48, 0);
    lv_obj_set_style_pad_gap(lv_screen_active(), 8, 0);

    lv_obj_t * title = lv_label_create(lv_layer_top());
    lv_obj_set_style_bg_opa(title, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(title, lv_color_white(), 0);
    lv_obj_set_style_text_color(title, lv_color_black(), 0);
    lv_obj_set_width(title, lv_pct(100));
    lv_label_set_text(title, "-");

    load_scene(scene_act);

    lv_timer_create(next_scene_timer_cb, SCENE_TIME, NULL);

    lv_subject_add_observer_obj(&sysmon_perf.subject, sysmon_perf_observer_cb, title, NULL);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


static void load_scene(uint32_t scene)
{
    lv_obj_t * scr = lv_screen_active();
    lv_obj_clean(scr);
    lv_obj_set_style_bg_color(scr, lv_palette_lighten(LV_PALETTE_GREY, 4), 0);
    lv_obj_set_style_text_color(scr, lv_color_black(), 0);
    lv_obj_set_style_pad_all(lv_screen_active(), 8, 0);
    lv_obj_set_style_pad_top(lv_screen_active(), 48, 0);
    lv_obj_set_style_pad_gap(lv_screen_active(), 8, 0);
    lv_obj_set_layout(scr, LV_LAYOUT_NONE);

    lv_anim_delete(scr, scroll_anim_y_cb);
    lv_anim_delete(scr, shake_anim_y_cb);
    lv_anim_delete(scr, color_anim_cb);

    lv_anim_delete(lv_layer_top(), color_anim_cb);
    lv_obj_set_style_bg_opa(lv_layer_top(), LV_OPA_TRANSP, 0);

    if(scenes[scene].create_cb) scenes[scene].create_cb();
}



static void next_scene_timer_cb(lv_timer_t * timer)
{
    LV_UNUSED(timer);

    scene_act++;
    load_scene(scene_act);
}

static void sysmon_perf_observer_cb(lv_subject_t * subject, lv_observer_t * observer)
{
    const lv_sysmon_perf_info_t * info = lv_subject_get_pointer(subject);
    lv_obj_t * label = lv_observer_get_target(observer);

    lv_label_set_text_fmt(label,
                          "%" LV_PRIu32" FPS, %" LV_PRIu32 "%% CPU\n"
                          "refr. %" LV_PRIu32" ms = %" LV_PRIu32 "ms render + %" LV_PRIu32" ms flush",
                          info->calculated.fps, info->calculated.cpu,
                          info->calculated.render_avg_time + info->calculated.flush_avg_time,
                          info->calculated.render_avg_time, info->calculated.flush_avg_time);
}

/*----------------
 * SCENE HELPERS
 *----------------*/


static void color_anim_cb(void * var, int32_t v)
{
    lv_obj_set_style_bg_color(var, lv_color_hex3(lv_rand(0x00f, 0xff0)), 0);
    lv_obj_set_style_text_color(var, lv_color_hex3(lv_rand(0x00f, 0xff0)), 0);
}

static void color_anim(lv_obj_t * obj)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, color_anim_cb);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_time(&a, 100);      /*New value in each ms*/
    lv_anim_set_var(&a, obj);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);
}

static void arc_anim_cb(void * var, int32_t v)
{
    lv_arc_set_value(var, v);
}

static void arc_anim(lv_obj_t * obj)
{
    uint32_t t1 = rnd_next(1000, 3000);
    uint32_t t2 = rnd_next(1000, 3000);
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, arc_anim_cb);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_time(&a, t1);
    lv_anim_set_playback_time(&a, t2);
    lv_anim_set_var(&a, obj);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);
}

static void scroll_anim_y_cb(void * var, int32_t v)
{
    lv_obj_scroll_to_y(var, v, LV_ANIM_OFF);
}

static void scroll_anim(lv_obj_t * obj, lv_coord_t y_max)
{
    uint32_t t1 = rnd_next(1000, 3000);
    uint32_t t2 = rnd_next(1000, 3000);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_exec_cb(&a, scroll_anim_y_cb);
    lv_anim_set_values(&a, 0, y_max);
    lv_anim_set_time(&a, t1);
    lv_anim_set_playback_time(&a, t2);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);

}
static void shake_anim_y_cb(void * var, int32_t v)
{
    lv_obj_set_style_translate_y(var, v, 0);
}

static void shake_anim(lv_obj_t * obj, lv_coord_t y_max)
{
    uint32_t t1 = rnd_next(300, 3000);
    uint32_t t2 = rnd_next(300, 3000);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_exec_cb(&a, shake_anim_y_cb);
    lv_anim_set_values(&a, 0, y_max);
    lv_anim_set_time(&a, t1);
    lv_anim_set_playback_time(&a, t2);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);
}


static lv_obj_t * card_create(void)
{
    lv_obj_t * panel = lv_obj_create(lv_screen_active());
    lv_obj_set_size(panel, 270, 120);
    lv_obj_set_style_pad_all(panel, 8, 0);

    LV_IMG_DECLARE(img_transform_avatar_15);
    lv_obj_t * child = lv_image_create(panel);
    lv_obj_align(child, LV_ALIGN_LEFT_MID, 0, 0);
    lv_image_set_src(child, &img_transform_avatar_15);

    child = lv_label_create(panel);
    lv_label_set_text(child, "John Smith");
    lv_obj_set_style_text_font(child, &lv_font_montserrat_24, 0);
    lv_obj_set_pos(child, 100, 0);

    child = lv_label_create(panel);
    lv_label_set_text(child, "A DIY enthusiast");
    lv_obj_set_style_text_font(child, &lv_font_montserrat_14, 0);
    lv_obj_set_pos(child, 100, 30);

    child = lv_button_create(panel);
    lv_obj_set_pos(child, 100, 50);

    child = lv_label_create(child);
    lv_label_set_text(child, "Connect");



    return panel;
}

static void rnd_reset(void)
{
    rnd_act = 0;
}

static int32_t rnd_next(int32_t min, int32_t max)
{
    static const uint32_t rnd_map[] = {
        0xbd13204f, 0x67d8167f, 0x20211c99, 0xb0a7cc05,
        0x06d5c703, 0xeafb01a7, 0xd0473b5c, 0xc999aaa2,
        0x86f9d5d9, 0x294bdb29, 0x12a3c207, 0x78914d14,
        0x10a30006, 0x6134c7db, 0x194443af, 0x142d1099,
        0x376292d5, 0x20f433c5, 0x074d2a59, 0x4e74c293,
        0x072a0810, 0xdd0f136d, 0x5cca6dbc, 0x623bfdd8,
        0xb645eb2f, 0xbe50894a, 0xc9b56717, 0xe0f912c8,
        0x4f6b5e24, 0xfe44b128, 0xe12d57a8, 0x9b15c9cc,
        0xab2ae1d3, 0xb4dc5074, 0x67d457c8, 0x8e46b00c,
        0xa29a1871, 0xcee40332, 0x80f93aa1, 0x85286096,
        0x09bd6b49, 0x95072088, 0x2093924b, 0x6a27328f,
        0xa796079b, 0xc3b488bc, 0xe29bcce0, 0x07048a4c,
        0x7d81bd99, 0x27aacb30, 0x44fc7a0e, 0xa2382241,
        0x8357a17d, 0x97e9c9cc, 0xad10ff52, 0x9923fc5c,
        0x8f2c840a, 0x20356ba2, 0x7997a677, 0x9a7f1800,
        0x35c7562b, 0xd901fe51, 0x8f4e053d, 0xa5b94923,
    };

    if(min == max) return min;

    if(min > max) {
        int32_t t = min;
        min = max;
        max = t;
    }

    int32_t d = max - min;
    int32_t r = (rnd_map[rnd_act] % d) + min;

    rnd_act++;
    if(rnd_act >= sizeof(rnd_map) / sizeof(rnd_map[0])) rnd_act = 0;

    return r;
}

#endif
