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

uniform float hs_angle1;
uniform float hs_angle2;

uniform float h_max;
uniform float h_min;

uniform float zscale;
//actual map values
uniform sampler2D tex;

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
        return texture2D(tex,texcoordn).r;
    }
}

void main()
{
        if(is_legend > 0)
        {
                float val = pow(texcoord.y,2.0);


                vec4 color = GetColorFromRamp(val);
                fragColor = vec4(mix(vec3(1.0,1.0,1.0),color.rgb,color.a),1.0);

        }else if(is_ldd > 0)
        {

            vec2 texcoordnormal;

            if(is_transformedf == 1)
            {
                texcoordnormal = GetTexCoordsWithTransform(texcoord);
            }else
            {
                texcoordnormal = GetTexCoords(texcoord);
            }

            vec2 texcoordn;
            vec2 texcoordcellnt;
            vec2 texcoordcellnb;
            vec2 texcoordcellnl;
            vec2 texcoordcellnr;
            vec2 texcoordcellntr;
            vec2 texcoordcellntl;
            vec2 texcoordcellnbr;
            vec2 texcoordcellnbl;

            if(is_transformedf == 1)
            {
                texcoordn = GetCellTexCoordsWithTransform(texcoord,0,0);
                texcoordcellnt = GetCellTexCoordsWithTransform(texcoord,0,1);
                texcoordcellnb = GetCellTexCoordsWithTransform(texcoord,0,-1);
                texcoordcellnl = GetCellTexCoordsWithTransform(texcoord,-1,0);
                texcoordcellnr = GetCellTexCoordsWithTransform(texcoord,1,0);
                texcoordcellntr = GetCellTexCoordsWithTransform(texcoord,1,1);
                texcoordcellntl = GetCellTexCoordsWithTransform(texcoord,-1,1);
                texcoordcellnbr = GetCellTexCoordsWithTransform(texcoord,1,-1);
                texcoordcellnbl = GetCellTexCoordsWithTransform(texcoord,-1,-1);
            }else
            {
                texcoordn = GetCellTexCoords(texcoord,0,0);
                texcoordcellnt = GetCellTexCoords(texcoord,0,1);
                texcoordcellnb = GetCellTexCoords(texcoord,0,-1);
                texcoordcellnl = GetCellTexCoords(texcoord,-1,0);
                texcoordcellnr = GetCellTexCoords(texcoord,1,0);
                texcoordcellntr = GetCellTexCoords(texcoord,1,1);
                texcoordcellntl = GetCellTexCoords(texcoord,-1,1);
                texcoordcellnbr = GetCellTexCoords(texcoord,1,-1);
                texcoordcellnbl = GetCellTexCoords(texcoord,-1,-1);
            }

            float val = GetTexValueAt(texcoordn);
            float valt = GetTexValueAt(texcoordcellnt);
            float valb = GetTexValueAt(texcoordcellnb);
            float vall = GetTexValueAt(texcoordcellnl);
            float valr = GetTexValueAt(texcoordcellnr);
            float valtr = GetTexValueAt(texcoordcellntr);
            float valtl = GetTexValueAt(texcoordcellntl);
            float valbr = GetTexValueAt(texcoordcellnbr);
            float valbl = GetTexValueAt(texcoordcellnbl);

            bool is_in = false;
            bool is_in_bound = false;
            bool is_in_bound2 = false;
            vec2 locincell = GetLocationWithinCell(texcoordnormal);

            if(locincell.x < 0.02 || locincell.x > 0.98 ||locincell.y < 0.02 || locincell.y > 0.98 )
            {
                is_in_bound = true;
            }

            float lwidth = 0.05;
            if(floor(val) == 5 && abs(locincell.y - 0.5) < lwidth * 3.0 && abs(locincell.x - 0.5) < lwidth * 3.0)
            {
                is_in = true;
            }

            if(floor(val+ 0.1) == 8 && locincell.y < 0.5 && abs(locincell.x - 0.5) < lwidth)
            {
                is_in = true;
            }
            if(floor(val+ 0.1) == 2 && locincell.y > 0.5 && abs(locincell.x - 0.5) < lwidth)
            {
                is_in = true;
            }
            if(floor(val+ 0.1) == 4 && locincell.x < 0.5 && abs(locincell.y - 0.5) < lwidth)
            {
                is_in = true;
            }
            if(floor(val+ 0.1) == 6 && locincell.x > 0.5 && abs(locincell.y - 0.5) < lwidth)
            {
                is_in = true;
            }


            if(floor(valt+ 0.1) == 2 && locincell.y < 0.5 && abs(locincell.x - 0.5) < lwidth)
            {
                is_in = true;
            }
            if(floor(valb+ 0.1) == 8 && locincell.y > 0.5 && abs(locincell.x - 0.5) < lwidth)
            {
                is_in = true;
            }
            if(floor(vall+ 0.1) == 6 && locincell.x < 0.5 && abs(locincell.y - 0.5) < lwidth)
            {
                is_in = true;
            }
            if(floor(valr+ 0.1) == 4 && locincell.x > 0.5 && abs(locincell.y - 0.5) < lwidth)
            {
                is_in = true;
            }

            if(floor(val+ 0.1) == 7 && locincell.x < 0.5 && locincell.y < 0.5 && abs(locincell.x - locincell.y)*sqrt(2.0) < lwidth)
            {
                is_in = true;
            }
            if(floor(val+ 0.1) == 9 && locincell.x > 0.5 && locincell.y < 0.5 && abs(locincell.x - (1.0-locincell.y))*sqrt(2.0)  < lwidth)
            {
                is_in = true;
            }
            if(floor(valtl+ 0.1) == 3 && locincell.x < 0.5 && locincell.y < 0.5 && abs(locincell.x - locincell.y)*sqrt(2.0) < lwidth)
            {
                is_in = true;
            }
            if(floor(valtr+ 0.1) == 1 && locincell.x > 0.5 && locincell.y < 0.5 && abs(locincell.x - (1.0-locincell.y))*sqrt(2.0)  < lwidth)
            {
                is_in = true;
            }


            if(floor(val+ 0.1) == 1 && locincell.x < 0.5 && locincell.y > 0.5 && abs(locincell.x - (1.0-locincell.y))*sqrt(2.0)  < lwidth)
            {
                is_in = true;
            }
            if(floor(val+ 0.1) == 3 && locincell.x > 0.5 && locincell.y > 0.5 && abs(locincell.x - locincell.y)*sqrt(2.0)  < lwidth)
            {
                is_in = true;
            }
            if(floor(valbl+ 0.1) == 9 && locincell.x < 0.5 && locincell.y > 0.5 && abs(locincell.x - (1.0-locincell.y))*sqrt(2.0)  < lwidth)
            {
                is_in = true;
            }
            if(floor(valbr+ 0.1) == 7 && locincell.x > 0.5 && locincell.y > 0.5 && abs(locincell.x - locincell.y)*sqrt(2.0)  < lwidth)
            {
                is_in = true;
            }


            float h_norm = (val - h_min)/(h_max -h_min);

            vec4 color = GetColorFromRamp(h_norm);
            if(texcoordn.x < 0 || texcoordn.x > 1 ||texcoordn.y < 0 || texcoordn.y > 1 )
            {
                fragColor = vec4(0.975,0.975,0.975,0.0);
            }else if(isnan(val))
            {
                fragColor = vec4(0.90,0.90,0.90,0.0);
            }else
            {
                if(is_in)
                {
                    fragColor = vec4(0.0,0.0,0.0,1.0);
                }else if(is_in_bound)
                {
                    fragColor = vec4(0.7,0.7,0.7,0.7);
                }else if(is_in_bound2)
                {
                    fragColor = vec4(0.4,0.4,0.4,0.7);
                }
                else
                {
                    fragColor = vec4(0.0,0.0,0.0,0.0);
                }
            }

        }else if(is_hs > 0)
        {
            vec2 texcoordnormal;

            if(is_transformedf == 1)
            {
                texcoordnormal = GetTexCoordsWithTransform(texcoord);
            }else
            {
                texcoordnormal = GetTexCoords(texcoord);
            }

            vec2 texcoordn;
            vec2 texcoordcellnt;
            vec2 texcoordcellnb;
            vec2 texcoordcellnl;
            vec2 texcoordcellnr;
            vec2 texcoordcellntr;
            vec2 texcoordcellntl;
            vec2 texcoordcellnbr;
            vec2 texcoordcellnbl;

            if(is_transformedf == 1)
            {
                texcoordn = GetCellTexCoordsWithTransform(texcoord,0,0);
                texcoordcellnt = GetCellTexCoordsWithTransform(texcoord,0,1);
                texcoordcellnb = GetCellTexCoordsWithTransform(texcoord,0,-1);
                texcoordcellnl = GetCellTexCoordsWithTransform(texcoord,-1,0);
                texcoordcellnr = GetCellTexCoordsWithTransform(texcoord,1,0);
                texcoordcellntr = GetCellTexCoordsWithTransform(texcoord,1,1);
                texcoordcellntl = GetCellTexCoordsWithTransform(texcoord,-1,1);
                texcoordcellnbr = GetCellTexCoordsWithTransform(texcoord,1,-1);
                texcoordcellnbl = GetCellTexCoordsWithTransform(texcoord,-1,-1);
            }else
            {
                texcoordn = GetCellTexCoords(texcoord,0,0);
                texcoordcellnt = GetCellTexCoords(texcoord,0,1);
                texcoordcellnb = GetCellTexCoords(texcoord,0,-1);
                texcoordcellnl = GetCellTexCoords(texcoord,-1,0);
                texcoordcellnr = GetCellTexCoords(texcoord,1,0);
                texcoordcellntr = GetCellTexCoords(texcoord,1,1);
                texcoordcellntl = GetCellTexCoords(texcoord,-1,1);
                texcoordcellnbr = GetCellTexCoords(texcoord,1,-1);
                texcoordcellnbl = GetCellTexCoords(texcoord,-1,-1);
            }

            float val = GetTexValueAt(texcoordn);
            float valt = GetTexValueAt(texcoordcellnt);
            float valb = GetTexValueAt(texcoordcellnb);
            float vall = GetTexValueAt(texcoordcellnl);
            float valr = GetTexValueAt(texcoordcellnr);
            float valtr = GetTexValueAt(texcoordcellntr);
            float valtl = GetTexValueAt(texcoordcellntl);
            float valbr = GetTexValueAt(texcoordcellnbr);
            float valbl = GetTexValueAt(texcoordcellnbl);


            float dx =(1.0/ul_pixx) *ul_sizex;
            float dy =(1.0/ul_pixy) *ul_sizey;

            float slopex = (valtl + 2.0f * valt + valtr - valbl - 2.0f * valb - valbr)/(8.0f*dx);
            float slopey = (valbr + 2.0f * valr + valtr - valbl - 2.0f * vall - valtl)/(8.0f*dy);

            vec3 normal = vec3(slopex,slopey,-1);
            vec3 light = vec3(-1,-1,-1);

            float z_factor = zscale;
            float Slope_rad = atan( sqrt ( slopex*slopex+slopey*slopey) );
            float Aspect_rad = 0;
            if( slopex != 0.0f)
            {
                Aspect_rad = atan(slopey, -slopex);
                if (Aspect_rad < 0)
                    Aspect_rad = 2.0f*3.14159 + Aspect_rad;
            }
            else
            {
                if(slopey > 0)
                {
                    Aspect_rad = 3.14159/2.0f;
                }
                else
                {
                    Aspect_rad = 2.0f*3.14159 - 3.14159/2.0f;
                }
            }

            //float l = max(0.0f,dot(normal,light));
            float Zenith_rad = hs_angle2* 3.14159/ 180.0f;
            float Azimuth_rad = hs_angle1 * 3.14159 / 180.0f;
            float l =   z_factor*(0.25+0.75*( cos(Zenith_rad) * cos(Slope_rad) ) + ( sin(Zenith_rad) * sin(Slope_rad) * cos(Azimuth_rad - Aspect_rad) ));
            vec4 color = vec4(l,l,l,1.0);

            if(texcoordn.x < 0 || texcoordn.x > 1 ||texcoordn.y < 0 || texcoordn.y > 1 )
            {
                fragColor = vec4(0.975,0.975,0.975,0.0);
                discard;
            }else if(isnan(val))
            {
                fragColor = vec4(0.90,0.90,0.90,0.0);
                discard;
            }else
            {

                fragColor = vec4(color.rgb,color.a * alpha);
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

            float val = texture2D(tex,texcoordn).r;

            if(is_raw > 0)
            {

                if(texcoordn.x < 0 || texcoordn.x > 1 ||texcoordn.y < 0 || texcoordn.y > 1 )
                {
                  fragColor = vec4(-1e30f,-1e30f,-1e30f,0.0);
                }else if(isnan(val))
                {
                    fragColor = vec4(-1e30f,-1e30f,-1e30f,0.0);
                }else
                {
                    fragColor = vec4(val * zscale,val* zscale,val * zscale,1.0);
                }
            }else
            {
                float h_norm = (val - h_min)/(h_max -h_min);

                vec4 color = GetColorFromRamp(h_norm);
                if(texcoordn.x < 0 || texcoordn.x > 1 ||texcoordn.y < 0 || texcoordn.y > 1 )
                {
                    fragColor = vec4(0.975,0.975,0.975,0.0);
                    discard;
                }else if(isnan(val))
                {
                    fragColor = vec4(0.90,0.90,0.90,0.0);
                    discard;
                }else
                {

                    fragColor = vec4(color.rgb,color.a * alpha);
                }


            }

        }

		
	//fragColor = vec4(texcoordn.x,texcoordn.y,0.0,1.0);
		
}



