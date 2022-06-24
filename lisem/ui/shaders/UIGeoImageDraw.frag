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

//actual map values
uniform sampler2D tex1;

//transformation properties (texture contains x and y coordinates)
uniform sampler2D texX;
uniform sampler2D texY;

uniform float alpha = 1.0;

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



void main()
{

            vec2 texcoordn;

            if(is_transformedf == 1)
            {
                texcoordn = GetTexCoordsWithTransform(texcoord);
            }else
            {
                texcoordn = GetTexCoords(texcoord);
            }

            vec4 color =texture2D(tex1,texcoordn).rgba;
            color.a = color.a * alpha;

            if(texcoordn.x < 0 || texcoordn.x > 1 ||texcoordn.y < 0 || texcoordn.y > 1 )
            {
                fragColor = vec4(0.975,0.975,0.975,0.0);
            }else if(isnan(color.r * color.b * color.g * color.a))
            {
                fragColor = vec4(0.90,0.90,0.90,0.0);
            }else
            {
                fragColor = texture2D(tex1,texcoordn).rgba;
                //fragColor = vec4(val1/255,val2/255,val3/255,alpha);
            }



}



