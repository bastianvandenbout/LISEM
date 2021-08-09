#version 400

in vec3 frag_position;
in vec2 frag_texcoords;
in vec3 frag_normal;

layout (location = 0) out vec4 frag_colour;
layout (location = 1) out vec4 frag_posx;
layout (location = 2) out vec4 frag_posy;
layout (location = 3) out vec4 frag_posz;

uniform highp vec3 ObjTranslate;
uniform highp vec3 ObjScale;
uniform highp int MarchingSteps = 35;

uniform highp vec3 CameraPosition;
uniform highp mat4 CMatrix;
uniform highp float SResolutionX;
uniform highp float SResolutionY;
uniform highp float iTime;

uniform highp vec3 iSunDir;
uniform highp int isSurface = 0;

uniform highp float iCloudCover;
uniform highp float iCloudHeight;
uniform highp float iCloudThickness;
uniform highp int iCloudIter;
uniform int isclouds;

uniform highp float hardness = 10.0f;

uniform highp vec2 cloudSize;
uniform highp float cloudRoughness;
uniform highp float cloudOpacity;

uniform sampler2D iChannel0;
uniform sampler2D iChannel1;

uniform sampler2D iPosX;
uniform sampler2D iPosY;
uniform sampler2D iPosZ;

uniform sampler3D iTex3D;

uniform float alpha = 1.0;

uniform float h_max;
uniform float h_min;

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

vec4 GetFirstNonTransColor()
{
    if(colorstop_c0.a > 0.9)
    {
        return colorstop_c0;
    }else if(colorstop_c1.a > 0.9)
    {
        return colorstop_c1;
    }else if(colorstop_c2.a > 0.9)
    {
        return colorstop_c2;
    }else if(colorstop_c3.a > 0.9)
    {
        return colorstop_c3;
    }else if(colorstop_c4.a > 0.9)
    {
        return colorstop_c4;
    }else if(colorstop_c5.a > 0.9)
    {
        return colorstop_c5;
    }else if(colorstop_c6.a > 0.9)
    {
        return colorstop_c6;
    }else if(colorstop_c7.a > 0.9)
    {
        return colorstop_c7;
    }else if(colorstop_c8.a > 0.9)
    {
        return colorstop_c8;
    }else if(colorstop_c9.a > 0.9)
    {
        return colorstop_c9;
    }
}
float GetFirstNonTransStop()
{
    if(colorstop_c0.a > 0.9)
    {
        return colorstop_0;
    }else if(colorstop_c1.a > 0.9)
    {
        return colorstop_1;
    }else if(colorstop_c2.a > 0.9)
    {
        return colorstop_2;
    }else if(colorstop_c3.a > 0.9)
    {
        return colorstop_3;
    }else if(colorstop_c4.a > 0.9)
    {
        return colorstop_4;
    }else if(colorstop_c5.a > 0.9)
    {
        return colorstop_5;
    }else if(colorstop_c6.a > 0.9)
    {
        return colorstop_6;
    }else if(colorstop_c7.a > 0.9)
    {
        return colorstop_7;
    }else if(colorstop_c8.a > 0.9)
    {
        return colorstop_8;
    }else if(colorstop_c9.a > 0.9)
    {
        return colorstop_9;
    }

}
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


bool IsInCube(vec3 boxmin, vec3 boxmax, vec3 pos)
{
    return (pos.x >= boxmin.x && pos.y >= boxmin.y && pos.z >= boxmin.z)
            && (pos.x <= boxmax.x && pos.y <= boxmax.y && pos.z <= boxmax.z);
}

float absmax(float min, float x)
{
    return x < -min? x : max(min,x);
}

vec3 GetCubeIntersects(vec3 boxmin, vec3 boxmax, vec3 ray)
{
    vec3 rayinv = vec3(1.0f/absmax(1e-10f,ray.x),1.0f/absmax(1e-10f,ray.y),1.0f/absmax(1e-10f,ray.z));

    vec3 tbot = rayinv * (boxmin);
    vec3 ttop = rayinv * (boxmax);

    vec3 tmin = min(ttop,tbot);
    vec3 tmax = max(ttop,tbot);
    vec2 t = max(tmin.xx, tmin.yz);
    float t0 = max(t.x,t.y);
    t = min(tmax.xx,tmax.yz);
    float t1 = min(t.x,t.y);
    return vec3(t0,t1,t1 > max(t0,0.0f));
}

float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}




void main() {

    vec2 viewportCoord = vec2(gl_FragCoord.x/SResolutionX,gl_FragCoord.y/SResolutionY); //ndc is -1 to 1 in GL. scale for 0 to 1

    //get current world position of pixel
    //NOTE: World position is relative to camera in x- and z- directions, as are all positions.
    //this is done to avoid floating point precision artifacts within textures and models
    //with this solution, the precision error only comes about at the relative position, and is orders of magnitude less visible.

    //see if the camera is inside or outside the box

    vec3 wpos =vec3(frag_position.x,frag_position.y-CameraPosition.y,frag_position.z);
    vec3 raydir = normalize(wpos);
    float maxlength = sqrt(ObjScale.x*ObjScale.x +ObjScale.y*ObjScale.y +ObjScale.z*ObjScale.z );


    //get next intersection with box

    vec4 surfcolor = GetFirstNonTransColor();
    float surfstop = GetFirstNonTransStop();

    vec3 intersects = GetCubeIntersects(ObjTranslate- vec3(0.0,CameraPosition.y,0.0),ObjTranslate + ObjScale- vec3(0.0,CameraPosition.y,0.0),raydir);

    bool cam_in = IsInCube(ObjTranslate - vec3(0.0,CameraPosition.y,0.0),ObjTranslate + ObjScale- vec3(0.0,CameraPosition.y,0.0),vec3(0.0,0.0,0.0));

    //set starting point of ray
    vec3 ray_start = cam_in? vec3(0.0,0.0,0.0) : intersects.x * raydir;

    //set stopping point of ray
    vec3 ray_end = intersects.y * raydir;

    vec3 dr = ray_end-ray_start;

    //random jutter to offset banding
    float random = rand(vec2(viewportCoord.x + iTime, viewportCoord.y * iTime));


        vec3 drstep = dr/(float(MarchingSteps) -2.0 - 0.5 * random * (1.0f-exp(-hardness/100.0f)) );

        //now we can do some marching
        //do the volume render equations, see also gpu gems

        float value = length(dr)/min(abs(ObjScale.x),min(abs(ObjScale.y),abs(ObjScale.z)));//maxlength;
        vec4 color = vec4(0.0f,0.0f,0.0f,0.0f);
        float dt = value/(float(MarchingSteps) - 4.0f);

        int n_div = 0;
        float step_div = 1.0f;
        int n_same = 0;

        float addmax = 0.0f;
        vec3 posmax = vec3(0.0f,0.0f,0.0f);
        float n_colorsteps = 0;
        vec4 colormax = vec4(0.0f,0.0f,0.0f,0.0f);

        vec3 r = ray_start - drstep;

        bool normalfromside= false;
        bool keep_color = false;
        float h_prev = 0.0;
        for(int i = 0; i < MarchingSteps; i++)
        {

            //step our ray
            r = r + drstep * step_div;


            //get texture coordinates
            vec3 p0 = (r -(ObjTranslate - vec3(0.0,CameraPosition.y,0.0)));

            vec3 p;
            //some mipmapping artifacts in top and bottom layer
            p.x = max(0.015,min(p0.x/ObjScale.x,0.985));
            p.z = max(0.015,min(p0.y/ObjScale.y,0.985));
            p.y = max(0.015,min(p0.z/ObjScale.z,0.985));

            //now get value from texture
            //should automatically use mipmaps
            float val = texture(iTex3D, p).r;
            float h_norm = (val - h_min)/(h_max -h_min);
            vec4 val_color = GetColorFromRamp(h_norm);
            val_color.a = val_color.a *min(1.0,dt *step_div * hardness);

            if(i > 0 &&  isSurface > 0 && h_norm > surfstop && h_prev < surfstop || keep_color)
            {
                val_color = surfcolor;
                keep_color = true;
            }
            h_prev = h_norm;

            if((1.0f - color.a) * val_color.a > addmax)
            {
                addmax = (1.0f - color.a) * val_color.a;
                posmax = p;
                colormax = color;
            }


            if((1.0f - color.a) * val_color.a > 0.5 && i == 0)
            {
                normalfromside = true;
            }

            //detect a harsh boundary
            if((1.0f - color.a) * val_color.a > 0.5f && ((float(i)) < (float(MarchingSteps)) * 0.8f) && i > 0 && n_div < 10)
            {
                r = r -  1.25f *drstep * step_div;
                n_div += 1;
                step_div = step_div * 0.5;
                n_same = 0;
                continue;
            }else
            {
                keep_color = false;
                n_colorsteps += 1.0;

                //vec4 val_color = vec4(val,val,val,dt);//vec4(p.x,p.y,p.z,p.x*p.y*p.z);//test with transparent texture coordinates

                //integrate the rendering equation
                color.rgb += (1.0f - color.a) * val_color.a*  val_color.rgb;
                color.a += (1.0f - color.a) * val_color.a;

                if (color.a >= 0.975f)
                {
                        break;
                }

                n_same += 1;

                if(n_same > 3 && n_div > 0)
                {
                    step_div = min(1.0f,step_div * 2.0f);
                    n_same = 0;
                    n_div -= 1;
                }
            }
        }

        //if we did not converge, but at least sampled some visible color, show that instead

        if(color.a < 0.05 && colormax.a > 10.0f*color.a)
        {
            color = colormax;
        }

        //if we have a potential impact position, lets calculate the normal so we can do approximate surface lighting


        if(isSurface > 0)
        {

            const float eps = 0.001;
            vec3 normal =normalize(vec3
                (    -(texture(iTex3D,posmax + vec3(0, 0, eps)    ).r - texture(iTex3D,posmax - vec3(0, 0, eps)).r),
                     -(texture(iTex3D,posmax + vec3(eps, 0, 0)    ).r - texture(iTex3D,posmax - vec3(eps, 0, 0)).r),
                    -(texture(iTex3D,posmax + vec3(0, eps, 0)    ).r - texture(iTex3D,posmax - vec3(0, eps, 0)).r)

                )
            );

            if(normalfromside)
            {
                vec3 closest = max(posmax,1.0-posmax);

                if(closest.x > closest.y && closest.x > closest.z)
                {
                    normal = vec3(posmax.x > 0.5? 1.0:-1.0,0.0,0.0);
                }
                if(closest.y > closest.z && closest.y > closest.x)
                {
                    normal = vec3(0.0,posmax.y > 0.5? 1.0:-1.0,0.0);
                }
                if(closest.z > closest.y && closest.z > closest.x)
                {
                    normal = vec3(0.0,0.0,posmax.z > 0.5? 1.0:-1.0);
                }
            }
            vec4 color_lighted = vec4(color.rgb * (0.4 +0.6 * max(0.0f,dot(normalize(normal),normalize(iSunDir)))),color.a);


            frag_colour = color_lighted;
        }else
        {
            frag_colour = 0.025 *(1.0-color.a) * vec4(value,value,value,value) + (0.975 + 0.025 *(color.a))* vec4(color.r,color.g,color.b,color.a * alpha);

        }
    //float testval = n_colorsteps/5.0f;
    //frag_colour = vec4(testval,testval,testval,1.0f);
    //frag_colour = vec4(vec3(1.0,0.702,0.278) * (0.4 +0.6 * max(0.0f,dot(normalize(frag_normal),normalize(iSunDir)))),1.0);

    frag_posx = vec4(frag_position.x,frag_position.x,frag_position.x,1.0);
    frag_posy = vec4(frag_position.y,frag_position.y,frag_position.y,1.0);
    frag_posz = vec4(frag_position.z,frag_position.z,frag_position.z,1.0) ;


}
