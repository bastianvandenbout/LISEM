#version 330


in highp vec2 texcoord;

out vec4 fragColor;


uniform mat4 matrix;

uniform float ul_sizex;
uniform float ul_sizey;
uniform float ul_pixx;
uniform float ul_pixy;
uniform float ul_pixdx;
uniform float ul_pixdy;
uniform float ul_transx;
uniform float ul_transy;

uniform float ug_windowpixsizex;
uniform float ug_windowpixsizey;
uniform float ug_windowsizex;
uniform float ug_windowsizey;
uniform float ug_windowhwration;
uniform float ug_windowtransx;
uniform float ug_windowtransy;

uniform float tr_sizex;
uniform float tr_sizey;
uniform float tr_transx;
uniform float tr_transy;

uniform int is_transformedf;
uniform int is_legend;
uniform int is_ldd;
uniform int is_raw;
uniform int is_hs;
uniform int is_vel;
uniform float vel_spacing;
uniform int is_fractional;

const float PI = 3.1415927;

const int   ARROW_V_STYLE = 1;
const int   ARROW_LINE_STYLE = 2;

// Choose your arrow head style
const int   ARROW_STYLE = 2;
const float ARROW_TILE_SIZE = 64.0;

// How sharp should the arrow head be? Used
const float ARROW_HEAD_ANGLE = 45.0 * PI / 180.0;

// Used for ARROW_LINE_STYLE
const float ARROW_HEAD_LENGTH_FRAC = 0.16;
const float ARROW_SHAFT_THICKNESS = 3.0;

uniform float h_max;
uniform float h_min;
uniform float h_max2;
uniform float h_min2;


uniform float zscale;
//actual map values
uniform sampler2D tex1;
uniform sampler2D tex2;

//transformation properties (texture contains x and y coordinates)
uniform sampler2D texX;
uniform sampler2D texY;

uniform float alpha = 1.0;

uniform float colorstop_0 = 0.0;
uniform float colorstop_1 = 0.001;
uniform float colorstop_2 = 0.01;
uniform float colorstop_3 = 0.025;
uniform float colorstop_4 = 0.05;
uniform float colorstop_5 = 0.1;
uniform float colorstop_6 = 0.25;
uniform float colorstop_7 = 0.5;
uniform float colorstop_8 = 0.75;
uniform float colorstop_9 = 1.0;
uniform vec4 colorstop_c0 = vec4(0.9019,0.9764,1.0,0.0);
uniform vec4 colorstop_c1 = vec4(0.9019,0.9764,1.0,0.25);
uniform vec4 colorstop_c2 = vec4(0.0,1.0,1.0,0.5);
uniform vec4 colorstop_c3 = vec4(0.0,0.8,1.0,0.75);
uniform vec4 colorstop_c4 = vec4(0.0,0.6,1.0,1.0);
uniform vec4 colorstop_c5 = vec4(0.0,0.2,0.8,1.0);
uniform vec4 colorstop_c6 = vec4(0.0,0.0,0.5,1.0);
uniform vec4 colorstop_c7 = vec4(1.0,0.0,0.0,1.0);
uniform vec4 colorstop_c8 = vec4(0.5,0.0,0.5,1.0);
uniform vec4 colorstop_c9 = vec4(0.0,0.0,0.0,1.0);

uniform float colorstop2_0 = 0.0;
uniform float colorstop2_1 = 0.001;
uniform float colorstop2_2 = 0.01;
uniform float colorstop2_3 = 0.025;
uniform float colorstop2_4 = 0.05;
uniform float colorstop2_5 = 0.1;
uniform float colorstop2_6 = 0.25;
uniform float colorstop2_7 = 0.5;
uniform float colorstop2_8 = 0.75;
uniform float colorstop2_9 = 1.0;
uniform vec4 colorstop2_c0 = vec4(0.9019,0.9764,1.0,0.0);
uniform vec4 colorstop2_c1 = vec4(0.9019,0.9764,1.0,0.25);
uniform vec4 colorstop2_c2 = vec4(0.0,1.0,1.0,0.5);
uniform vec4 colorstop2_c3 = vec4(0.0,0.8,1.0,0.75);
uniform vec4 colorstop2_c4 = vec4(0.0,0.6,1.0,1.0);
uniform vec4 colorstop2_c5 = vec4(0.0,0.2,0.8,1.0);
uniform vec4 colorstop2_c6 = vec4(0.0,0.0,0.5,1.0);
uniform vec4 colorstop2_c7 = vec4(1.0,0.0,0.0,1.0);
uniform vec4 colorstop2_c8 = vec4(0.5,0.0,0.5,1.0);
uniform vec4 colorstop2_c9 = vec4(0.0,0.0,0.0,1.0);

vec4 GetColorFromRamp(float val)
{
	if(val > colorstop_0)
	{
		if(val < colorstop_1)
		{
			return mix(colorstop_c0,colorstop_c1,(val - colorstop_0)/(colorstop_1 - colorstop_0));
		}else if(val < colorstop_2)
		{
			return mix(colorstop_c1,colorstop_c2,(val - colorstop_1)/(colorstop_2 - colorstop_1));
		}else if(val < colorstop_3)
		{
			return mix(colorstop_c2,colorstop_c3,(val - colorstop_2)/(colorstop_3 - colorstop_2));
		}else if(val < colorstop_4)
		{
			return mix(colorstop_c3,colorstop_c4,(val - colorstop_3)/(colorstop_4 - colorstop_3));
		}else if(val < colorstop_5)
		{
			return mix(colorstop_c4,colorstop_c5,(val - colorstop_4)/(colorstop_5 - colorstop_4));
		}else if(val < colorstop_6)
		{
			return mix(colorstop_c5,colorstop_c6,(val - colorstop_5)/(colorstop_6 - colorstop_5));
		}else if(val < colorstop_7)
		{
			return mix(colorstop_c6,colorstop_c7,(val - colorstop_6)/(colorstop_7 - colorstop_6));
		}else if(val < colorstop_8)
		{
			return mix(colorstop_c7,colorstop_c8,(val - colorstop_7)/(colorstop_8 - colorstop_7));
		}else if(val < colorstop_9)
		{
			return mix(colorstop_c8,colorstop_c9,(val - colorstop_8)/(colorstop_9 - colorstop_8));
		}else
		{
			return colorstop_c9;
		}
	}else
	{
	return colorstop_c0;
	}
}

vec4 GetColorFromRamp2(float val)
{
        if(val > colorstop2_0)
        {
                if(val < colorstop2_1)
                {
                        return mix(colorstop2_c0,colorstop2_c1,(val - colorstop2_0)/(colorstop2_1 - colorstop2_0));
                }else if(val < colorstop2_2)
                {
                        return mix(colorstop2_c1,colorstop2_c2,(val - colorstop2_1)/(colorstop2_2 - colorstop2_1));
                }else if(val < colorstop2_3)
                {
                        return mix(colorstop2_c2,colorstop2_c3,(val - colorstop2_2)/(colorstop2_3 - colorstop2_2));
                }else if(val < colorstop2_4)
                {
                        return mix(colorstop2_c3,colorstop2_c4,(val - colorstop2_3)/(colorstop2_4 - colorstop2_3));
                }else if(val < colorstop2_5)
                {
                        return mix(colorstop2_c4,colorstop2_c5,(val - colorstop2_4)/(colorstop2_5 - colorstop2_4));
                }else if(val < colorstop2_6)
                {
                        return mix(colorstop2_c5,colorstop2_c6,(val - colorstop2_5)/(colorstop2_6 - colorstop2_5));
                }else if(val < colorstop2_7)
                {
                        return mix(colorstop2_c6,colorstop2_c7,(val - colorstop2_6)/(colorstop2_7 - colorstop2_6));
                }else if(val < colorstop2_8)
                {
                        return mix(colorstop2_c7,colorstop2_c8,(val - colorstop2_7)/(colorstop2_8 - colorstop2_7));
                }else if(val < colorstop2_9)
                {
                        return mix(colorstop2_c8,colorstop2_c9,(val - colorstop2_8)/(colorstop2_9 - colorstop2_8));
                }else
                {
                        return colorstop2_c9;
                }
        }else
        {
        return colorstop2_c0;
        }
}

vec2 GetTexCoords(vec2 vin)
{
    vec2 texcoord = vin;
    vec2 texcoordn = vec2(texcoord.x,1.0-texcoord.y);

    if(ul_sizex < 0)
    {
        texcoordn = vec2(1.0-texcoordn.x,texcoordn.y);
    }
    if(ul_sizey < 0)
    {
        texcoordn = vec2(texcoordn.x,1.0-texcoordn.y);
    }



    return texcoordn;
}

vec2 GetTexCoordsWithTransform(vec2 vin)
{

    vec2 texcoord = vin;


    vec2 texcoordt = texcoord;
    float xcoordtf = texture2D(texX,texcoordt).r;
    float ycoordtf = texture2D(texY,texcoordt).r;

    float tf_tlx = tr_transx - 0.5 * tr_sizex;
    float tf_brx = tr_transx + 0.5 * tr_sizex;
    float tf_tly = tr_transy - 0.5 * tr_sizey;
    float tf_bry = tr_transy + 0.5 * tr_sizey;

    float ul_tlx = ul_transx - 0.5 * ul_sizex;
    float ul_brx = ul_transx + 0.5 * ul_sizex;
    float ul_tly = ul_transy - 0.5 * ul_sizey;
    float ul_bry = ul_transy + 0.5 * ul_sizey;

    vec2 texcoordn = vec2(((xcoordtf - ul_tlx)/ul_sizex),((ycoordtf - ul_tly)/ul_sizey));
    texcoordn = vec2(texcoordn.x,1.0-texcoordn.y);
    if(ul_sizex < 0)
    {
        texcoordn = vec2(1.0-texcoordn.x,texcoordn.y);
    }
    if(ul_sizey < 0)
    {
        texcoordn = vec2(texcoordn.x,1.0-texcoordn.y);
    }

    //float x_coord_tf_norm = (xcoordtf - tf_tlx)/(tf_brx -tf_tlx);
    //float y_coord_tf_norm = (ycoordtf - tf_tly)/(tf_bry -tf_tly);

    //fragColor = vec4(texcoordn.x,texcoordn.y,color.a,1.0);
    //to color the coordinate transformation
    //vec4(0.0,x_coord_tf_norm,y_coord_tf_norm,1.0);


    return texcoordn;
}

vec2 GetCellTexCoords(vec2 vin, int cx, int cy)
{
    vec2 texcoord = vin;
    vec2 texcoordn = vec2(texcoord.x,1.0-texcoord.y);

    if(ul_sizex < 0)
    {
        texcoordn = vec2(1.0-texcoordn.x,texcoordn.y);
    }
    if(ul_sizey < 0)
    {
        texcoordn = vec2(texcoordn.x,1.0-texcoordn.y);
    }


    texcoordn.x = texcoordn.x - mod(texcoordn.x,1.0/ul_pixx)+ 0.5 * 1.0/ul_pixx + cx * 1.0/ul_pixx;
    texcoordn.y = texcoordn.y - mod(texcoordn.y,1.0/ul_pixy)+ 0.5 * 1.0/ul_pixy- cy * 1.0/ul_pixy;

    return texcoordn;
}

vec2 GetCellTexCoordsWithTransform(vec2 vin, int cx, int cy)
{

    vec2 texcoord = vin;


    vec2 texcoordt = texcoord;
    float xcoordtf = texture2D(texX,texcoordt).r;
    float ycoordtf = texture2D(texY,texcoordt).r;

    float tf_tlx = tr_transx - 0.5 * tr_sizex;
    float tf_brx = tr_transx + 0.5 * tr_sizex;
    float tf_tly = tr_transy - 0.5 * tr_sizey;
    float tf_bry = tr_transy + 0.5 * tr_sizey;

    float ul_tlx = ul_transx - 0.5 * ul_sizex;
    float ul_brx = ul_transx + 0.5 * ul_sizex;
    float ul_tly = ul_transy - 0.5 * ul_sizey;
    float ul_bry = ul_transy + 0.5 * ul_sizey;

    vec2 texcoordn = vec2(((xcoordtf - ul_tlx)/ul_sizex),((ycoordtf - ul_tly)/ul_sizey));
    texcoordn = vec2(texcoordn.x,1.0-texcoordn.y);
    if(ul_sizex < 0)
    {
        texcoordn = vec2(1.0-texcoordn.x,texcoordn.y);
    }
    if(ul_sizey < 0)
    {
        texcoordn = vec2(texcoordn.x,1.0-texcoordn.y);
    }

    //float x_coord_tf_norm = (xcoordtf - tf_tlx)/(tf_brx -tf_tlx);
    //float y_coord_tf_norm = (ycoordtf - tf_tly)/(tf_bry -tf_tly);

    //fragColor = vec4(texcoordn.x,texcoordn.y,color.a,1.0);
    //to color the coordinate transformation
    //vec4(0.0,x_coord_tf_norm,y_coord_tf_norm,1.0);


    texcoordn.x = texcoordn.x - mod(texcoordn.x,1.0/ul_pixx)+ 0.5 * 1.0/ul_pixx + cx * 1.0/ul_pixx;
    texcoordn.y = texcoordn.y - mod(texcoordn.y,1.0/ul_pixy)+ 0.5 * 1.0/ul_pixy - cy * 1.0/ul_pixy;

    return texcoordn;
}

vec2 GetLocationWithinCell(vec2 texcoordn)
{
    vec2 texcoord;
    texcoord.x = mod(texcoordn.x,1.0/ul_pixx) * ul_pixx;
    texcoord.y = mod(texcoordn.y,1.0/ul_pixy) * ul_pixy;
    return texcoord;
}

float GetTexValueAt(vec2 texcoordn)
{
    if(texcoordn.x < 0 || texcoordn.x > 1 ||texcoordn.y < 0 || texcoordn.y > 1 )
    {
        return -1;
    }else
    {
        return texture2D(tex1,texcoordn).r;
    }
}


vec2 arrowTileCenterCoord(vec2 pos) {
        return (floor(pos / vel_spacing ) + 0.5) * vel_spacing ;
}


// Computes the signed distance from a line segment
float line(vec2 p, vec2 p1, vec2 p2) {
        vec2 center = (p1 + p2) * 0.5;
        float len = length(p2 - p1);
        vec2 dir = (p2 - p1) / len;
        vec2 rel_p = p - center;
        float dist1 = abs(dot(rel_p, vec2(dir.y, -dir.x)));
        float dist2 = abs(dot(rel_p, dir)) - 0.5*len;
        return max(dist1, dist2);
}

// v = field sampled at arrowTileCenterCoord(p), scaled by the length
// desired in pixels for arrows
// Returns a signed distance from the arrow
float arrow(vec2 p, vec2 v) {
        // Make everything relative to the center, which may be fractional
        p -= arrowTileCenterCoord(p);

        float mag_v = length(v), mag_p = length(p);

        if (mag_v > 0.0) {
                // Non-zero velocity case
                vec2 dir_v = v / mag_v;

                // We can't draw arrows larger than the tile radius, so clamp magnitude.
                // Enforce a minimum length to help see direction
                mag_v = clamp(mag_v, 0.0, vel_spacing);

                // Arrow tip location
                v = dir_v * mag_v;

                // Signed distance from shaft
                float shaft = line(p, v, -v);
                // Signed distance from head
                float head = min(line(p, v, 0.4*v + 0.2*vec2(-v.y, v.x)),
                                 line(p, v, 0.4*v + 0.2*vec2(v.y, -v.x)));

                return min(shaft, head);
        } else {
                // Signed distance from the center point
                return mag_p;
        }
}


/*
float arrow(vec2 p, vec2 v) {
        // Make everything relative to the center, which may be fractional
        p -= arrowTileCenterCoord(p);

    float mag_v = length(v), mag_p = length(p);

        if (mag_v > 0.0) {
                // Non-zero velocity case
                vec2 dir_p = p / mag_p, dir_v = v / mag_v;

                // We can't draw arrows larger than the tile radius, so clamp magnitude.
                // Enforce a minimum length to help see direction
                mag_v = clamp(mag_v, 5.0,  vel_spacing / 2.0);

                // Arrow tip location
                v = dir_v * mag_v;

                // Define a 2D implicit surface so that the arrow is antialiased.
                // In each line, the left expression defines a shape and the right controls
                // how quickly it fades in or out.

                float dist;
                if (ARROW_STYLE == ARROW_LINE_STYLE) {
                        // Signed distance from a line segment based on https://www.shadertoy.com/view/ls2GWG by
                        // Matthias Reitinger, @mreitinger

                        // Line arrow style
                        dist =
                                max(
                                        // Shaft
                                        ARROW_SHAFT_THICKNESS / 4.0 -
                                                max(abs(dot(p, vec2(dir_v.y, -dir_v.x))), // Width
                                                    abs(dot(p, dir_v)) - mag_v + (vel_spacing*ARROW_HEAD_LENGTH_FRAC) / 2.0), // Length

                                 // Arrow head
                                         min(0.0, dot(v - p, dir_v) - cos(ARROW_HEAD_ANGLE / 2.0) * length(v - p)) * 2.0 + // Front sides
                                         min(0.0, dot(p, dir_v) +  (vel_spacing*ARROW_HEAD_LENGTH_FRAC) - mag_v)); // Back
                } else {
                        // V arrow style
                        dist = min(0.0, mag_v - mag_p) * 2.0 + // length
                                   min(0.0, dot(normalize(v - p), dir_v) - cos(ARROW_HEAD_ANGLE / 2.0)) * 2.0 * length(v - p) + // head sides
                                   min(0.0, dot(p, dir_v) + 1.0) + // head back
                                   min(0.0, cos(ARROW_HEAD_ANGLE / 2.0) - dot(normalize(v * 0.33 - p), dir_v)) * mag_v * 0.8; // cutout
                }

                return clamp(1.0 + dist, 0.0, 1.0);
        } else {
                // Center of the pixel is always on the arrow
                return max(0.0, 1.2 - mag_p);
        }
}*/

vec2 field(vec2 pos) {
        return vec2(cos(pos.x * 0.01 + pos.y * 0.01) + cos(pos.y * 0.005 ), 2.0 * cos(pos.y * 0.01 )) * 0.5;

}

void main()
{
        if(is_legend > 0)
        {
                float val = pow(texcoord.y,2.0);


                vec4 color1 = GetColorFromRamp(val);
                vec4 color2 = GetColorFromRamp2(val);

                vec4 color = mix(color1,color2,texcoord.x);

                fragColor = vec4(mix(vec3(1.0,1.0,1.0),color.rgb,color.a),1.0);

        }else
        {

            if(is_vel == 1)
            {
                float arrowval = 0.0;

                if(is_transformedf == 1)
                {
                    vec2 pixcoord = gl_FragCoord.xy;
                    //pixcoord.x = pixcoord.x * ug_windowpixsizex;
                    //pixcoord.y = pixcoord.y * ug_windowpixsizey;
                    vec2 center = arrowTileCenterCoord(pixcoord.xy);
                    vec2 texcoordn = center;
                    texcoordn.x = texcoordn.x / (ug_windowpixsizex);
                    texcoordn.y = texcoordn.y / (ug_windowpixsizey);

                    if(is_transformedf == 1)
                    {
                        texcoordn = GetTexCoordsWithTransform(texcoordn);
                    }else
                    {
                        texcoordn = GetTexCoords(texcoordn);
                    }

                    float val = texture2D(tex1,texcoordn).r;
                    float val2 = texture2D(tex2,texcoordn).r;

                    if(is_fractional == 1)
                    {
                        val2 = val2/(1e-30 +val + val2);
                    }
                    float hmax_all = max(-h_min,max(-h_min2,max(h_max,h_max2)));

                    if(texcoordn.x < 0 || texcoordn.x > 1 ||texcoordn.y < 0 || texcoordn.y > 1 )
                    {
                        fragColor = vec4(0.975,0.975,0.975,0.0);
                        discard;
                    }else if(isnan(val) || isnan(val2))
                    {
                        fragColor = vec4(0.90,0.90,0.90,0.0);
                        discard;
                    }
                    float h_norm = (sqrt(val*val + val2*val2))/(hmax_all);

                    vec4 color1 = GetColorFromRamp(h_norm);

                    vec2 fieldv = normalize(vec2(val,-val2))*h_norm;
                    arrowval = 1.0-arrow(pixcoord.xy, fieldv * vel_spacing* 0.4);
                    fragColor = vec4(color1.x,color1.y,color1.z,color1.w * arrowval* alpha);
                }else
                {
                    vec2 pixcoord = texcoord.xy;
                    pixcoord.x = pixcoord.x * (ul_sizex/ug_windowsizex)* ug_windowpixsizex;
                    pixcoord.y = pixcoord.y * (ul_sizey/ug_windowsizey)* ug_windowpixsizey;
                    vec2 center = arrowTileCenterCoord(pixcoord.xy);
                    vec2 texcoordn = center;
                    texcoordn.x = texcoordn.x / ((ul_sizex/ug_windowsizex)* ug_windowpixsizex);
                    texcoordn.y = texcoordn.y / ((ul_sizey/ug_windowsizey)* ug_windowpixsizey);

                    if(is_transformedf == 1)
                    {
                        texcoordn = GetTexCoordsWithTransform(texcoordn);
                    }else
                    {
                        texcoordn = GetTexCoords(texcoordn);
                    }


                    float val = texture2D(tex1,texcoordn).r;
                    float val2 = texture2D(tex2,texcoordn).r;

                    float hmax_all = max(-h_min,max(-h_min2,max(h_max,h_max2)));

                    if(is_fractional == 1)
                    {
                        val2 = val2/(1e-30 +val + val2);
                    }
                    if(texcoordn.x < 0 || texcoordn.x > 1 ||texcoordn.y < 0 || texcoordn.y > 1 )
                    {
                        fragColor = vec4(0.975,0.975,0.975,0.0);
                        discard;
                    }else if(isnan(val) || isnan(val2))
                    {
                        fragColor = vec4(0.90,0.90,0.90,0.0);
                        discard;
                    }
                    float h_norm = (sqrt(val*val + val2*val2))/(hmax_all);

                    vec4 color1 = GetColorFromRamp(h_norm);

                    vec2 fieldv = normalize(vec2(val,-val2))*h_norm;

                    arrowval = 1.0-arrow(pixcoord.xy, fieldv * vel_spacing* 0.4);
                    fragColor = vec4(color1.x,color1.y,color1.z,color1.w * arrowval* alpha);
                }


            }else
            {
                vec2 texcoordn;

                if(is_transformedf == 1)
                {
                    texcoordn = GetTexCoordsWithTransform(texcoord);
                }else
                {
                    texcoordn = GetTexCoords(texcoord);
                }

                float val = texture2D(tex1,texcoordn).r;
                float val2 = texture2D(tex2,texcoordn).r;

                {
                    float h_norm = (val - h_min)/(h_max -h_min);
                    float h_norm2 = (val2 - h_min2)/(h_max2 -h_min2);


                    vec4 color1 = GetColorFromRamp(h_norm);
                    vec4 color2 = GetColorFromRamp2(h_norm);

                    vec4 color;


                    if(val2 < 1e-30)
                    {
                        h_norm2 = 0.0;
                        color = color1;
                    }else
                    {
                       color = mix(color1,color2,h_norm2);
                    }
                    if(texcoordn.x < 0 || texcoordn.x > 1 ||texcoordn.y < 0 || texcoordn.y > 1 )
                    {
                        fragColor = vec4(0.975,0.975,0.975,0.0);
                        discard;
                    }else if(isnan(val) || isnan(val2))
                    {
                        fragColor = vec4(0.90,0.90,0.90,0.0);
                        discard;
                    }else
                    {

                            fragColor = vec4(color.rgb,color1.a * alpha);

                    }
                }
            }

        }

}



