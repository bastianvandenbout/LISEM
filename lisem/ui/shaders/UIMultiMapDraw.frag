#version 330


in vec2 texcoord;

out vec4 fragColor;


uniform mat4 matrix;

uniform float ul_pixx;
uniform float ul_pixy;
uniform float ul_pixdx;
uniform float ul_pixdy;

uniform float ul_sizex;
uniform float ul_sizey;
uniform float ul_transx;
uniform float ul_transy;

uniform float ul_sizex1;
uniform float ul_sizey1;
uniform float ul_transx1;
uniform float ul_transy1;

uniform float ul_sizex2;
uniform float ul_sizey2;
uniform float ul_transx2;
uniform float ul_transy2;

uniform float ul_relscale1;
uniform float ul_relshiftx1;
uniform float ul_relshifty1;

uniform float ul_relscale2;
uniform float ul_relshiftx2;
uniform float ul_relshifty2;

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

uniform float tr_sizex1;
uniform float tr_sizey1;
uniform float tr_transx1;
uniform float tr_transy1;

uniform float tr_sizex2;
uniform float tr_sizey2;
uniform float tr_transx2;
uniform float tr_transy2;


uniform int is_transformedf;
uniform int is_legend;
uniform int is_ldd;

uniform float h1max;
uniform float h1min;
uniform float h2max;
uniform float h2min;
uniform float h3max;
uniform float h3min;

uniform int has_band1;
uniform int has_band2;
uniform int has_band3;

//actual map values
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;

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

vec2 GetTexCoords1(vec2 vin)
{
    vec2 texcoord = vin;
    vec2 texcoordn = texcoord;

    if(ul_sizex < 0 && ul_sizey < 0)
    {
                texcoordn = (texcoordn - vec2(((ul_transx1 - 0.5f * ul_sizex1)-(ul_transx-0.5 * ul_sizex))/ul_sizex,((ul_transy1 - 0.5f * ul_sizey1)-(ul_transy-0.5 * ul_sizey))/ul_sizey)) *( ul_sizex1/ul_sizex);
        texcoordn = vec2(1.0-texcoordn.x,texcoordn.y);
                texcoordn = vec2(texcoordn.x,1.0-texcoordn.y);
    } else if(ul_sizey < 0)
    {
                texcoordn = (texcoordn - vec2(((ul_transx1 - 0.5f * ul_sizex1)-(ul_transx-0.5 * ul_sizex))/ul_sizex,((ul_transy1 - 0.5f * ul_sizey1)-(ul_transy-0.5 * ul_sizey))/ul_sizey)) *( ul_sizex1/ul_sizex);
    }else if(ul_sizex < 0 )
        {
                texcoordn = (texcoordn - vec2(((ul_transx1 - 0.5f * ul_sizex1)-(ul_transx-0.5 * ul_sizex))/ul_sizex,((ul_transy1 - 0.5f * ul_sizey1)-(ul_transy-0.5 * ul_sizey))/ul_sizey)) *( ul_sizex1/ul_sizex);
        texcoordn = vec2(1.0-texcoordn.x,texcoordn.y);
        }else{
                texcoordn = (texcoordn - vec2(((ul_transx1 - 0.5f * ul_sizex1)-(ul_transx-0.5 * ul_sizex))/ul_sizex,((ul_transy1 - 0.5f * ul_sizey1)-(ul_transy-0.5 * ul_sizey))/ul_sizey)) *( ul_sizex1/ul_sizex);
            texcoordn = vec2(texcoordn.x,1.0-texcoordn.y);
        }

    return texcoordn;
}


vec2 GetTexCoords2(vec2 vin)
{
    vec2 texcoord = vin;
    vec2 texcoordn = texcoord;

        if(ul_sizex < 0 && ul_sizey < 0)
    {
                texcoordn = (texcoordn - vec2(((ul_transx2 - 0.5f * ul_sizex2)-(ul_transx-0.5 * ul_sizex))/ul_sizex,((ul_transy2 - 0.5f * ul_sizey1)-(ul_transy-0.5 * ul_sizey))/ul_sizey)) *( ul_sizex2/ul_sizex);
        texcoordn = vec2(1.0-texcoordn.x,texcoordn.y);
                texcoordn = vec2(texcoordn.x,1.0-texcoordn.y);
    } else if(ul_sizey < 0)
    {
                texcoordn = (texcoordn - vec2(((ul_transx2 - 0.5f * ul_sizex2)-(ul_transx-0.5 * ul_sizex))/ul_sizex,((ul_transy2 - 0.5f * ul_sizey1)-(ul_transy-0.5 * ul_sizey))/ul_sizey)) *( ul_sizex2/ul_sizex);
    }else if(ul_sizex < 0 )
        {
                texcoordn = (texcoordn - vec2(((ul_transx2 - 0.5f * ul_sizex2)-(ul_transx-0.5 * ul_sizex))/ul_sizex,((ul_transy2 - 0.5f * ul_sizey1)-(ul_transy-0.5 * ul_sizey))/ul_sizey)) *( ul_sizex2/ul_sizex);
        texcoordn = vec2(1.0-texcoordn.x,texcoordn.y);
        }else{
                texcoordn = (texcoordn - vec2(((ul_transx2 - 0.5f * ul_sizex2)-(ul_transx-0.5 * ul_sizex))/ul_sizex,((ul_transy2 - 0.5f * ul_sizey1)-(ul_transy-0.5 * ul_sizey))/ul_sizey)) *( ul_sizex2/ul_sizex);
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

    return texcoordn;
}

vec2 GetTexCoordsWithTransform1(vec2 vin)
{

    vec2 texcoord = vin;


    vec2 texcoordt = texcoord;
    float xcoordtf = texture2D(texX,texcoordt).r;
    float ycoordtf = texture2D(texY,texcoordt).r;

    float tf_tlx = tr_transx1 - 0.5 * tr_sizex1;
    float tf_brx = tr_transx1 + 0.5 * tr_sizex1;
    float tf_tly = tr_transy1 - 0.5 * tr_sizey1;
    float tf_bry = tr_transy1 + 0.5 * tr_sizey1;

    float ul_tlx = ul_transx1 - 0.5 * ul_sizex1;
    float ul_brx = ul_transx1 + 0.5 * ul_sizex1;
    float ul_tly = ul_transy1 - 0.5 * ul_sizey1;
    float ul_bry = ul_transy1 + 0.5 * ul_sizey1;

    vec2 texcoordn = vec2(((xcoordtf - ul_tlx)/ul_sizex1),((ycoordtf - ul_tly)/ul_sizey1));
    texcoordn = vec2(texcoordn.x,1.0-texcoordn.y);
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


vec2 GetTexCoordsWithTransform2(vec2 vin)
{

    vec2 texcoord = vin;


    vec2 texcoordt = texcoord;
    float xcoordtf = texture2D(texX,texcoordt).r;
    float ycoordtf = texture2D(texY,texcoordt).r;

    float tf_tlx = tr_transx2 - 0.5 * tr_sizex2;
    float tf_brx = tr_transx2 + 0.5 * tr_sizex2;
    float tf_tly = tr_transy2 - 0.5 * tr_sizey2;
    float tf_bry = tr_transy2 + 0.5 * tr_sizey2;

    float ul_tlx = ul_transx2 - 0.5 * ul_sizex2;
    float ul_brx = ul_transx2 + 0.5 * ul_sizex2;
    float ul_tly = ul_transy2 - 0.5 * ul_sizey2;
    float ul_bry = ul_transy2 + 0.5 * ul_sizey2;

    vec2 texcoordn = vec2(((xcoordtf - ul_tlx)/ul_sizex2),((ycoordtf - ul_tly)/ul_sizey2));
    texcoordn = vec2(texcoordn.x,1.0-texcoordn.y);
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
    texcoordn.x = texcoordn.x - mod(texcoordn.x,1.0/ul_pixx)+ 0.5 * 1.0/ul_pixx + cx * 1.0/ul_pixx;
    texcoordn.y = texcoordn.y - mod(texcoordn.y,1.0/ul_pixy)+ 0.5 * 1.0/ul_pixy - cy * 1.0/ul_pixy;

    return texcoordn;
}

vec2 GetCellTexCoordsWithTransform1(vec2 vin, int cx, int cy)
{

    vec2 texcoord = vin;


    vec2 texcoordt = texcoord;
    float xcoordtf = texture2D(texX,texcoordt).r;
    float ycoordtf = texture2D(texY,texcoordt).r;

    float tf_tlx = tr_transx1 - 0.5 * tr_sizex1;
    float tf_brx = tr_transx1 + 0.5 * tr_sizex1;
    float tf_tly = tr_transy1 - 0.5 * tr_sizey1;
    float tf_bry = tr_transy1 + 0.5 * tr_sizey1;

    float ul_tlx = ul_transx1 - 0.5 * ul_sizex1;
    float ul_brx = ul_transx1 + 0.5 * ul_sizex1;
    float ul_tly = ul_transy1 - 0.5 * ul_sizey1;
    float ul_bry = ul_transy1 + 0.5 * ul_sizey1;

    vec2 texcoordn = vec2(((xcoordtf - ul_tlx)/ul_sizex1),((ycoordtf - ul_tly)/ul_sizey1));
    texcoordn = vec2(texcoordn.x,1.0-texcoordn.y);
    if(ul_sizex < 0)
    {
        texcoordn = vec2(1.0-texcoordn.x,texcoordn.y);
    }
    if(ul_sizey < 0)
    {
        texcoordn = vec2(texcoordn.x,1.0-texcoordn.y);
    }
    texcoordn.x = texcoordn.x - mod(texcoordn.x,1.0/ul_pixx)+ 0.5 * 1.0/ul_pixx + cx * 1.0/ul_pixx;
    texcoordn.y = texcoordn.y - mod(texcoordn.y,1.0/ul_pixy)+ 0.5 * 1.0/ul_pixy - cy * 1.0/ul_pixy;

    return texcoordn;
}

vec2 GetCellTexCoordsWithTransform2(vec2 vin, int cx, int cy)
{

    vec2 texcoord = vin;


    vec2 texcoordt = texcoord;
    float xcoordtf = texture2D(texX,texcoordt).r;
    float ycoordtf = texture2D(texY,texcoordt).r;

    float tf_tlx = tr_transx2 - 0.5 * tr_sizex2;
    float tf_brx = tr_transx2 + 0.5 * tr_sizex2;
    float tf_tly = tr_transy2 - 0.5 * tr_sizey2;
    float tf_bry = tr_transy2 + 0.5 * tr_sizey2;

    float ul_tlx = ul_transx2 - 0.5 * ul_sizex2;
    float ul_brx = ul_transx2 + 0.5 * ul_sizex2;
    float ul_tly = ul_transy2 - 0.5 * ul_sizey2;
    float ul_bry = ul_transy2 + 0.5 * ul_sizey2;

    vec2 texcoordn = vec2(((xcoordtf - ul_tlx)/ul_sizex2),((ycoordtf - ul_tly)/ul_sizey2));
    texcoordn = vec2(texcoordn.x,1.0-texcoordn.y);
    if(ul_sizex < 0)
    {
        texcoordn = vec2(1.0-texcoordn.x,texcoordn.y);
    }
    if(ul_sizey < 0)
    {
        texcoordn = vec2(texcoordn.x,1.0-texcoordn.y);
    }
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

float GetTex1ValueAt(vec2 texcoordn)
{
    if(texcoordn.x < 0 || texcoordn.x > 1 ||texcoordn.y < 0 || texcoordn.y > 1 )
    {
        return -1;
    }else
    {
        return texture2D(tex1,texcoordn).r;
    }
}

float GetTex2ValueAt(vec2 texcoordn)
{
    if(texcoordn.x < 0 || texcoordn.x > 1 ||texcoordn.y < 0 || texcoordn.y > 1 )
    {
        return -1;
    }else
    {
        return texture2D(tex2,texcoordn).r;
    }
}


float GetTex3ValueAt(vec2 texcoordn)
{
    if(texcoordn.x < 0 || texcoordn.x > 1 ||texcoordn.y < 0 || texcoordn.y > 1 )
    {
        return -1;
    }else
    {
        return texture2D(tex3,texcoordn).r;
    }
}


void main()
{
        if(is_legend > 0)
        {
                float val = pow(texcoord.y,2.0);


                vec4 color = GetColorFromRamp(val);
                fragColor = vec4(mix(vec3(1.0,1.0,1.0),color.rgb,color.a),1.0);

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

            vec2 texcoordn1;

            if(is_transformedf == 1)
            {
                texcoordn1 = GetTexCoordsWithTransform1(texcoord);
            }else
            {
                texcoordn1 = GetTexCoords1(texcoord);
            }


            vec2 texcoordn2;

            if(is_transformedf == 1)
            {
                texcoordn2 = GetTexCoordsWithTransform2(texcoord);
            }else
            {
                texcoordn2 = GetTexCoords2(texcoord);
            }


            float val1 = texture2D(tex1,texcoordn).r;
            float val2 = texture2D(tex2,texcoordn1).r;
            float val3 = texture2D(tex3,texcoordn2).r;

            float v1_norm = (val1 - h1min)/(h1max -h1min);
            float v2_norm = (val2 - h2min)/(h2max -h2min);
            float v3_norm = (val3 - h3min)/(h3max -h3min);

            if(texcoordn.x < 0 || texcoordn.x > 1 ||texcoordn.y < 0 || texcoordn.y > 1 )
            {
                fragColor = vec4(0.975,0.975,0.975,0.0);
            }else if(isnan(val1) || isnan(val2) || isnan(val3))
            {
                fragColor = vec4(0.90,0.90,0.90,0.0);
            }else
            {
                fragColor = vec4(v1_norm,v2_norm,v3_norm,alpha);
                //fragColor = vec4(val1/255,val2/255,val3/255,alpha);
            }


        }
}



