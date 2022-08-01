

#include "model.h"

#include "mapviewtool.h"


#include "extensions/scriptarrayhelpers.h"

MapViewTool * CMapViewToolManager;



void MapViewTool::SetScriptFunctions(ScriptManager * sm)
{

    std::cout << "add viewlayer calls " << this << std::endl;

    int r = sm->m_Engine->RegisterObjectType("UILayer", sizeof(ASUILayer), asOBJ_VALUE | asOBJ_POD| asOBJ_APP_CLASS_ALLINTS|asGetTypeTraits<ASUILayer>());
    sm->m_Engine->RegisterObjectBehaviour("UILayer", asBEHAVE_CONSTRUCT, "void CSF0()", asFUNCTIONPR(AS_ASUILayerC0,(void*),void), asCALL_CDECL_OBJLAST);
    sm->m_Engine->RegisterObjectBehaviour("UILayer", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(AS_ASUILayerD0), asCALL_CDECL_OBJLAST);
    sm->m_Engine->RegisterObjectMethod("UILayer", "UILayer& opAssign(const UILayer &in m)", asMETHODPR(ASUILayer,Assign,(ASUILayer *),ASUILayer*), asCALL_THISCALL); assert( r >= 0 );

    r = sm->m_Engine->RegisterObjectType("UIPlot", sizeof(ASUIPlot), asOBJ_VALUE | asOBJ_POD| asOBJ_APP_CLASS_ALLINTS|asGetTypeTraits<ASUIPlot>());
    sm->m_Engine->RegisterObjectBehaviour("UIPlot", asBEHAVE_CONSTRUCT, "void CSF0()", asFUNCTIONPR(AS_ASUIPlotC0,(void*),void), asCALL_CDECL_OBJLAST);
    sm->m_Engine->RegisterObjectBehaviour("UIPlot", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(AS_ASUIPlotD0), asCALL_CDECL_OBJLAST);
    sm->m_Engine->RegisterObjectMethod("UIPlot", "UIPlot& opAssign(const UIPlot &in m)", asMETHODPR(ASUIPlot,Assign,(ASUIPlot *),ASUIPlot*), asCALL_THISCALL); assert( r >= 0 );

    r = sm->m_Engine->RegisterObjectType("UIPlotLayer", sizeof(ASUIPlotLayer), asOBJ_VALUE | asOBJ_POD| asOBJ_APP_CLASS_ALLINTS|asGetTypeTraits<ASUIPlotLayer>());
    sm->m_Engine->RegisterObjectBehaviour("UIPlotLayer", asBEHAVE_CONSTRUCT, "void CSF0()", asFUNCTIONPR(AS_ASUIPlotLayerC0,(void*),void), asCALL_CDECL_OBJLAST);
    sm->m_Engine->RegisterObjectBehaviour("UIPlotLayer", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(AS_ASUIPlotLayerD0), asCALL_CDECL_OBJLAST);
    sm->m_Engine->RegisterObjectMethod("UIPlotLayer", "UIPlotLayer& opAssign(const UIPlotLayer &in m)", asMETHODPR(ASUIPlotLayer,Assign,(ASUIPlotLayer *),ASUIPlotLayer*), asCALL_THISCALL); assert( r >= 0 );


    //Create Plot
    sm->m_Engine->RegisterGlobalFunction("UIPlot CreatePlot(string name)", asMETHODPR( MapViewTool ,CreatePlot, (QString),ASUIPlot),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    sm->m_Engine->RegisterGlobalFunction("UIPlotLayer AddPlotLayer(UIPlot p, const Table &in t, string name)", asMETHODPR( MapViewTool ,AddPlotLayer, (ASUIPlot, MatrixTable *, QString),ASUIPlotLayer),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    sm->m_Engine->RegisterGlobalFunction("void ReplacePlotLayer(UIPlotLayer l, const Table &in t)", asMETHODPR( MapViewTool ,ReplacePlotLayer, (ASUIPlotLayer,MatrixTable * ),void),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    sm->m_Engine->RegisterGlobalFunction("void SetPlotLayerStyle(UIPlotLayer l, vec3 color)", asMETHODPR( MapViewTool ,SetPlotLayerStyle, (ASUIPlotLayer, LSMVector3),void),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );

    //add layer

    sm->m_Engine->RegisterGlobalFunction("UILayer AddViewLayerVelocity(Map &in mapx, Map &in mapy, string name, bool removeable = true)", asMETHODPR( MapViewTool ,AddVelLayerFromScript,(cTMap *,cTMap *,QString,bool),ASUILayer),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );

    //sm->m_Engine->RegisterGlobalFunction("void ReplaceViewLayerVelocity(UILayer l,Map &in mapx, Map &in mapy)", asMETHODPR( MapViewTool ,ReplaceVelLayerFromScript,(ASUILayer,cTMap *,cTMap *),void),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    //sm->m_Engine->RegisterGlobalFunction("void SetVelocityLayerRange(UILayer l,float min, float max)", asMETHODPR( MapViewTool ,AddVelLayerFromScript,(cTMap *,cTMap *,QString,bool),ASUILayer),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    //sm->m_Engine->RegisterGlobalFunction("void SetVelocityLayerColors(UILayer l,string name)", asMETHODPR( MapViewTool ,AddVelLayerFromScript,(cTMap *,cTMap *,QString,bool),ASUILayer),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );

    sm->m_Engine->RegisterGlobalFunction("UILayer AddViewLayer(Map &in map, string name, bool removeable = true)", asMETHODPR( MapViewTool ,AddLayerFromScript,(cTMap *,QString,bool),ASUILayer),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );

    //get map view layer is edited
    sm->m_Engine->RegisterGlobalFunction("bool IsViewLayerEdited(UILayer l, bool reset = true)", asMETHODPR( MapViewTool ,AS_IsViewLayerEdited,(ASUILayer),bool),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    sm->m_Engine->RegisterGlobalFunction("Map @GetEditedMap(UILayer l)", asMETHODPR( MapViewTool ,AS_GetEditedMap,(ASUILayer),cTMap *),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );

    //sm->m_Engine->RegisterGlobalFunction("UILayer AddViewLayer(Map &in map, string name, bool removeable = true)", asMETHODPR( MapViewTool ,AddLayerFromScript,(cTMap *,QString,bool),ASUILayer),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );


    //sm->m_Engine->RegisterGlobalSTDFunction("UILayer AddShaderLayer(QString text, int resolutionx, int resolutiony, const array<array<Map>> &in textures, Region region = Region(0.0,1.0,0.0,1.0), GeoProjection p = GeoProjectionGeneric(),bool isabs = false, bool dynamic = true,bool scaling = true, bool is2d = true)",GetFuncConvert(std::bind(&MapViewTool::AddCustomShader1,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::placeholders::_4,std::placeholders::_5, std::placeholders::_6,std::placeholders::_7,std::placeholders::_8,std::placeholders::_9,std::placeholders::_10) );
    sm->m_Engine->RegisterGlobalFunction("string rf(string file)",asMETHODPR( MapViewTool ,rf,(QString),QString),  asCALL_THISCALL_ASGLOBAL,this );


    //get mouse geo events
    //get mouse key events
    //
    //

    //m_Engine->RegisterFuncdef("void CALLBACKDFSBB(string, bool, bool)");
    //m_Engine->RegisterFuncdef("void CALLBACKDFSDDBB(string, double,double,bool, bool)");

    sm->m_Engine->RegisterGlobalFunction("bool IsMouseInScreen()", asMETHODPR( MapViewTool ,IsMouseInScreen,(),bool),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    sm->m_Engine->RegisterGlobalFunction("bool IsMouseButtonPressed(string name)", asMETHODPR( MapViewTool ,IsMouseButtonPressed,(QString),bool),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    sm->m_Engine->RegisterGlobalFunction("bool IsKeyPressed(string name)", asMETHODPR( MapViewTool ,IsKeyPressed,(QString),bool),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    sm->m_Engine->RegisterGlobalFunction("vec2 MousePosition()", asMETHODPR( MapViewTool ,MousePosition,(),LSMVector2),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    sm->m_Engine->RegisterGlobalFunction("vec3 MouseGeoPosition()", asMETHODPR( MapViewTool ,MouseGeoPosition,(),LSMVector3),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    sm->m_Engine->RegisterGlobalFunction("vec3 CameraPosition()", asMETHODPR( MapViewTool ,CameraPosition,(),LSMVector3),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    sm->m_Engine->RegisterGlobalFunction("vec3 ViewDirection()", asMETHODPR( MapViewTool ,ViewDirection,(),LSMVector3),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    sm->m_Engine->RegisterGlobalFunction("Region Camera2DView()", asMETHODPR( MapViewTool ,Camera2DView,(),BoundingBox),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    sm->m_Engine->RegisterGlobalFunction("void SetCamera2DView(Region r)", asMETHODPR( MapViewTool ,SetCamera2DView,(BoundingBox),void),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    sm->m_Engine->RegisterGlobalFunction("void SetCamera3DView(vec3 pos, vec3 view)", asMETHODPR( MapViewTool ,SetCamera3DView,(LSMVector3,LSMVector3),void),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    sm->m_Engine->RegisterGlobalFunction("void IsCamera3D()", asMETHODPR( MapViewTool ,IsCamera3D,(),bool),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    sm->m_Engine->RegisterGlobalFunction("void SetUI(bool edge, bool legends)", asMETHODPR( MapViewTool , SetUI,(bool,bool),void),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    sm->m_Engine->RegisterGlobalFunction("void SetUISize(float size_mult)", asMETHODPR( MapViewTool ,SetUISize,(float),void),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    sm->m_Engine->RegisterGlobalFunction("void DeleteAllViewLayers()", asMETHODPR( MapViewTool ,DeleteAllScriptLayers,(),void),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    sm->m_Engine->RegisterGlobalFunction("void DeleteAllLayers()", asMETHODPR( MapViewTool ,DeleteAllScriptLayers,(),void),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    sm->m_Engine->RegisterGlobalFunction("void MouseEventCall(CALLBACKDFSDDBB @func)", asMETHODPR( MapViewTool ,CallBackMouseScript,(asIScriptFunction *),void),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    sm->m_Engine->RegisterGlobalFunction("void KeyEventCall(CALLBACKDFSDDBB @func)", asMETHODPR( MapViewTool ,CallBackKeyScript,(asIScriptFunction *),void),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    sm->m_Engine->RegisterGlobalFunction("void OpenFullScreen(bool open = true, bool alternative_screen = false)", asMETHODPR( MapViewTool ,OpenFullScreen,(bool, bool),void),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    sm->m_Engine->RegisterGlobalFunction("vec2 GetScreenResolution()", asMETHODPR( MapViewTool ,GetScreenResolution,(),LSMVector2),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );


    sm->m_Engine->RegisterGlobalFunction("UILayer AddTextLayer(string text, vec3 position, float size = 12, vec4 color = vec4(0.0,0.0,0.0,1.0), GeoProjection &in p = GeoProjectionGeneric(), bool rel = false, bool center = false)", asMETHODPR( MapViewTool ,AddTextLayer,(QString,LSMVector3,float,LSMVector4,GeoProjection*, bool,bool),ASUILayer),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    sm->m_Engine->RegisterGlobalFunction("void SetTextShadow(const UILayer &in l, bool x, vec4 color,float size_incr = 2)", asMETHODPR( MapViewTool ,SetTextShadow,(ASUILayer,bool, LSMVector4, float),void),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    sm->m_Engine->RegisterGlobalFunction("void SetText(const UILayer &in l, string text)", asMETHODPR( MapViewTool ,SetText,(ASUILayer,QString),void),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    sm->m_Engine->RegisterGlobalFunction("void SetTextColor(const UILayer &in l, vec4 color)", asMETHODPR( MapViewTool ,SetTextColor,(ASUILayer,LSMVector4),void),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    sm->m_Engine->RegisterGlobalFunction("void SetTextPosition(const UILayer &in l, vec3 position, GeoProjection &in p = GeoProjectionGeneric(), bool rel = false)", asMETHODPR( MapViewTool ,SetTextPosition,(ASUILayer,LSMVector3,GeoProjection*,bool),void),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    sm->m_Engine->RegisterGlobalFunction("void SetTextSize(const UILayer &in l, string name, bool removeable = true)", asMETHODPR( MapViewTool ,SetTextSize,(ASUILayer,float),void),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );

    sm->m_Engine->RegisterGlobalFunction("UILayer AddImageLayer(string file, Region pos, GeoProjection &in p = GeoProjectionGeneric(), bool rel = false, int fillmode = 0)", asMETHODPR( MapViewTool ,AddImageLayer,(QString,BoundingBox,GeoProjection *,bool,int),ASUILayer),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    sm->m_Engine->RegisterGlobalFunction("UILayer AddImageLayer(const Map &in r, const Map &in g,const Map &in b,const Map &in a, Region pos, GeoProjection &in p = GeoProjectionGeneric(), bool rel = false,int fillmode = 0)", asMETHODPR( MapViewTool ,AddImageLayer2,(cTMap * ,cTMap * ,cTMap * ,cTMap * ,BoundingBox,GeoProjection*, bool,int),ASUILayer),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    sm->m_Engine->RegisterGlobalFunction("void SetImagePosition(const UILayer &in l, Region pos, GeoProjection &in p = GeoProjectionGeneric(), bool rel = false)", asMETHODPR( MapViewTool ,SetImagePosition,(ASUILayer,BoundingBox, GeoProjection*,bool),void),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    sm->m_Engine->RegisterGlobalFunction("void SetImagePositionRotation(const UILayer &in l, Region pos, float angle_degrees = 0, GeoProjection &in p = GeoProjectionGeneric(), bool rel = false)", asMETHODPR( MapViewTool ,SetImagePositionRotation,(ASUILayer,BoundingBox,float, GeoProjection*,bool),void),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    sm->m_Engine->RegisterGlobalFunction("void SetImage(const UILayer &in l, string file)", asMETHODPR( MapViewTool ,SetImageImage,(ASUILayer,QString),void),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    sm->m_Engine->RegisterGlobalFunction("void SetImage(const UILayer &in l, const Map &in r, const Map &in g,const Map &in b,const Map &in a)", asMETHODPR( MapViewTool ,SetImageImage2,(ASUILayer,cTMap *,cTMap *,cTMap *, cTMap *),void),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );


    sm->m_Engine->RegisterGlobalSTDFunction("UILayer AddShaderLayer(string shader, array<Map> &in images, Region pos,float resx = -1, float resy = -1, GeoProjection &in p = GeoProjectionGeneric(), bool rel = false)", GetFuncConvert(CAddCustomShaderLayer1));
    sm->m_Engine->RegisterGlobalSTDFunction("UILayer AddShaderLayer(string shader, array<string> &in images, Region pos,float resx = -1, float resy = -1, GeoProjection &in p = GeoProjectionGeneric(), bool rel = false)", GetFuncConvert(CAddCustomShaderLayer2));
    sm->m_Engine->RegisterGlobalSTDFunction("UILayer AddShaderLayer(array<string> &in shader, array<array<Map>> &in images, Region pos,float resx = -1, float resy = -1, GeoProjection &in p = GeoProjectionGeneric(), bool rel = false)", GetFuncConvert(CAddCustomShaderLayer1mp));
    sm->m_Engine->RegisterGlobalSTDFunction("UILayer AddShaderLayer(array<string> &in shader, array<array<string>> &in images,  Region  pos,float resx = -1, float resy = -1, GeoProjection &in p = GeoProjectionGeneric(), bool rel = false)", GetFuncConvert(CAddCustomShaderLayer2mp));
    sm->m_Engine->RegisterGlobalFunction("void SetShaderAutoScale(const UILayer &in l, bool scale = true)", asMETHODPR( MapViewTool ,SetShaderAutoScale,(ASUILayer, bool),void),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    sm->m_Engine->RegisterGlobalFunction("void SetShaderPosition(const UILayer &in l, Region pos, GeoProjection &in p = GeoProjectionGeneric(), bool rel = false)", asMETHODPR( MapViewTool ,SetShaderPosition,(ASUILayer,BoundingBox,GeoProjection*, bool),void),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    sm->m_Engine->RegisterGlobalFunction("void SetShaderInputFloats(const UILayer &in l, int i,float a, float b, float c,float d)", asMETHODPR( MapViewTool ,SetShaderInputFloats,(ASUILayer, int , float, float, float, float),void),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    sm->m_Engine->RegisterGlobalFunction("void SetShaderInputImage(const UILayer &in l, string file,int i = 0,int pass  = 0)", asMETHODPR( MapViewTool ,SetShaderInputImage,(ASUILayer, QString, int, int),void),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    sm->m_Engine->RegisterGlobalFunction("void SetShaderInputMap(const UILayer &in l,  const Map &in m,int i = 0,int pass  = 0)", asMETHODPR( MapViewTool ,SetShaderInputMap,(ASUILayer,cTMap *, int, int),void),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );


    //ask for input
    r = sm->m_Engine->RegisterGlobalFunction("UILayer AddViewLayer(Shapes &in shapes, string name, bool removeable = true)", asMETHODPR( MapViewTool ,AddLayerFromScript,(ShapeFile *,QString,bool),ASUILayer),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    sm->m_Engine->RegisterGlobalFunction("UILayer AddViewLayer(Field &in map, string name, bool removeable =  true)", asMETHODPR( MapViewTool ,AddLayerFromScript,(Field*,QString,bool),ASUILayer),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    sm->m_Engine->RegisterGlobalFunction("UILayer AddViewLayer(const RigidModel &in map, string name, bool removeable = true)", asMETHODPR( MapViewTool ,AddLayerFromScript,(RigidPhysicsWorld *,QString,bool),ASUILayer),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );

    //set shapes View Layer properties
    sm->m_Engine->RegisterGlobalFunction("void SetVectorFillStyle(UILayer layer, vec4 color1,float size = 2.0,float seperation = 7.0, float angle = 0.0, vec4 color2 = vec4(0.0,0.0,0.0,0.0), int pattern = 0)", asMETHODPR( MapViewTool ,SetLayerFillStyle,(ASUILayer, LSMVector4,float,float,float,LSMVector4,int),void),  asCALL_THISCALL_ASGLOBAL,this);
    sm->m_Engine->RegisterGlobalFunction("void SetVectorLineStyle(UILayer layer,vec4 color1, float size = 2.0,  vec4 color2 = vec4(0.0,0.0,0.0,0.0), int pattern = 0)", asMETHODPR( MapViewTool ,SetLayerLineStyle,(ASUILayer, LSMVector4,float,LSMVector4,int),void),  asCALL_THISCALL_ASGLOBAL,this);

    sm->m_Engine->RegisterGlobalFunction("void SetLayerTransparency(UILayer layer, float transparency)", asMETHODPR( MapViewTool ,SetLayerTransparency,(ASUILayer, float),void),  asCALL_THISCALL_ASGLOBAL,this);


    //remove layer
    r = sm->m_Engine->RegisterGlobalFunction("void RemoveViewLayer(UILayer layer)", asMETHODPR( MapViewTool ,RemoveLayerFromScript,(ASUILayer),void),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );

    //replace layer
    r = sm->m_Engine->RegisterGlobalFunction("void ReplaceViewLayer(UILayer layer, Map &in map)", asMETHODPR( MapViewTool ,ReplaceLayerFromScript,(ASUILayer, cTMap *),void),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    r = sm->m_Engine->RegisterGlobalFunction("void ReplaceViewLayer(UILayer layer, Shapes &in shapes)", asMETHODPR( MapViewTool ,ReplaceLayerFromScript,(ASUILayer, ShapeFile *),void),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    r = sm->m_Engine->RegisterGlobalFunction("void ReplaceViewLayer(UILayer layer, Field &in shapes)", asMETHODPR( MapViewTool ,ReplaceLayerFromScript,(ASUILayer, Field *),void),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );


    //camera control
    r = sm->m_Engine->RegisterGlobalFunction("void SetLimited2DView(Region r, float minimumscale = 0.5)", asMETHODPR( MapViewTool ,LimitWorldWindowView2D,(BoundingBox , float),void),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    r = sm->m_Engine->RegisterGlobalFunction("void SetFixed2DView(Region r)", asMETHODPR( MapViewTool ,FixWorldWindowView2D,(BoundingBox),void),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    r = sm->m_Engine->RegisterGlobalFunction("void SetFreeView(bool freeview = true)", asMETHODPR( MapViewTool ,SetFreeView,(bool),void),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    r = sm->m_Engine->RegisterGlobalFunction("void SetAllowShift(bool allow= true)", asMETHODPR( MapViewTool ,SetAllowShift,(bool),void),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );


    sm->m_Engine->RegisterGlobalFunction("void SetLayerHillshade(UILayer layer, bool hillshade, float angle_vert = 70.0, float angle_hor = 240.0)", asMETHODPR( MapViewTool ,SetLayerStyleHillShade,(ASUILayer, bool,float,float),void),  asCALL_THISCALL_ASGLOBAL,this);

    sm->m_Engine->RegisterGlobalFunction("void SetLayerValueRange(UILayer layer, double min, double max, int band = 1)", asMETHODPR( MapViewTool ,SetLayerValueRange,(ASUILayer l,double, double, int),void),  asCALL_THISCALL_ASGLOBAL,this);

    sm->m_Engine->RegisterGlobalFunction("void SetLayerColorGradient(UILayer layer, int index)", asMETHODPR( MapViewTool ,SetLayerStyleColorGradient,(ASUILayer l, int),void),  asCALL_THISCALL_ASGLOBAL,this);
    sm->m_Engine->RegisterGlobalFunction("void SetLayerColorGradient(UILayer layer, string name)", asMETHODPR( MapViewTool ,SetLayerStyleColorGradient,(ASUILayer l, QString),void),  asCALL_THISCALL_ASGLOBAL,this);
    sm->m_Engine->RegisterGlobalSTDFunction("void SetLayerColorGradient(UILayer layer,array<float> &in stops, array<vec4> &in colors )", GetFuncConvert(CSetLayerStyleColorGradient));

    //Get Layer Id Map
    r = sm->m_Engine->RegisterGlobalFunction("UILayer GetUILayer(string name)", asMETHODPR( MapViewTool ,GetLayerMap,(QString),ASUILayer),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );

    //get actual layer from id

    r = sm->m_Engine->RegisterGlobalFunction("Map& GetViewLayerMap(UILayer layer)", asMETHODPR( MapViewTool ,ScriptGetLayerMap,(ASUILayer),cTMap*),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );
    r = sm->m_Engine->RegisterGlobalFunction("Shapes& GetViewLayerShapes(UILayer layer)", asMETHODPR( MapViewTool ,ScriptGetLayerShapeFile,(ASUILayer),ShapeFile*),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );



    //define zones
    r = sm->m_Engine->RegisterObjectType("Zone",0,asOBJ_REF);//Shapes

    //register reference counting for garbage collecting
    r = sm->m_Engine->RegisterObjectBehaviour("Zone",asBEHAVE_ADDREF,"void f()",asMETHOD(ASZone2D,AS_AddRef),asCALL_THISCALL); assert( r >= 0 );
    r = sm->m_Engine->RegisterObjectBehaviour("Zone",asBEHAVE_RELEASE,"void f()",asMETHOD(ASZone2D,AS_ReleaseRef),asCALL_THISCALL); assert( r >= 0 );
    r = sm->m_Engine->RegisterObjectMethod("Zone", "Zone& opAssign(Zone &in m)", asMETHOD(ASZone2D,AS_Assign), asCALL_THISCALL); assert( r >= 0 );

    //register constructors by using factory functions
    r = sm->m_Engine->RegisterObjectBehaviour("Zone",asBEHAVE_FACTORY,"Zone@ CSF0()",asFUNCTIONPR(AS_ZoneFactory,(),ASZone2D *),asCALL_CDECL); assert( r >= 0 );

    r = sm->m_Engine->RegisterObjectMethod("Zone", "void SetPoint(vec2 pos)", asMETHODPR(ASZone2D,SetAsPointZone,(LSMVector2),void), asCALL_THISCALL); assert( r >= 0 );


    r = sm->m_Engine->RegisterGlobalFunction("Zone @GetPointZone(vec2 point)", asFUNCTIONPR(AS_GetPointZone,(LSMVector2),ASZone2D *),asCALL_CDECL); assert( r >= 0 );
    r = sm->m_Engine->RegisterGlobalFunction("Zone @GetBoxZone(Region r)", asFUNCTIONPR(AS_GetBoxZone,(BoundingBox),ASZone2D *),asCALL_CDECL); assert( r >= 0 );
    r = sm->m_Engine->RegisterGlobalFunction("Zone @GetDiscZone(vec2 point, float r, float r_inner = 0.0)", asFUNCTIONPR(AS_GetDiscZone,(LSMVector2,float,float),ASZone2D *),asCALL_CDECL); assert( r >= 0 );
    r = sm->m_Engine->RegisterGlobalFunction("Zone @GetDiscZoneGaussian(vec2 point, float r)", asFUNCTIONPR(AS_GetDiscZoneGaussian,(LSMVector2, float),ASZone2D *),asCALL_CDECL); assert( r >= 0 );
    r = sm->m_Engine->RegisterGlobalFunction("Zone @GetDiscSectorZone(vec2 point, float r, float r_inner = 0.0, float theta_min = 0.0, float theta_max = 6.28318530718)", asFUNCTIONPR(AS_GetDiscSectorZone,(LSMVector2,float,float,float,float),ASZone2D *),asCALL_CDECL); assert( r >= 0 );
    //r = sm->m_Engine->RegisterGlobalFunction("Zone @GetPolygonZone(array<vec2> &in point)", asFUNCTIONPR(AS_GetPolygonZone,(LSMVector2),ASZone2D *),asCALL_CDECL); assert( r >= 0 );
    r = sm->m_Engine->RegisterGlobalFunction("Zone @GetRasterZone(const Map &in)", asFUNCTIONPR(AS_GetRasterZone,(cTMap *),ASZone2D *),asCALL_CDECL); assert( r >= 0 );


    //Particle Effect class
    r = sm->m_Engine->RegisterObjectType("ParticleEffect",0,asOBJ_REF);//Shapes

    //register reference counting for garbage collecting
    r = sm->m_Engine->RegisterObjectBehaviour("ParticleEffect",asBEHAVE_ADDREF,"void f()",asMETHOD(ASEmitter,AS_AddRef),asCALL_THISCALL); assert( r >= 0 );
    r = sm->m_Engine->RegisterObjectBehaviour("ParticleEffect",asBEHAVE_RELEASE,"void f()",asMETHOD(ASEmitter,AS_ReleaseRef),asCALL_THISCALL); assert( r >= 0 );
    r = sm->m_Engine->RegisterObjectMethod("ParticleEffect", "ParticleEffect& opAssign(Zone &in m)", asMETHOD(ASEmitter,AS_Assign), asCALL_THISCALL); assert( r >= 0 );

    //register constructors by using factory functions
    r = sm->m_Engine->RegisterObjectBehaviour("ParticleEffect",asBEHAVE_FACTORY,"ParticleEffect@ CSF0()",asFUNCTIONPR(AS_ParticleEffectFactory,(),ASEmitter *),asCALL_CDECL); assert( r >= 0 );


    //add initializers
    //r = sm->m_Engine->RegisterObjectMethod("ParticleEffect", "void AddInitiatorPosition(Zone &in z, float weight = 1.0)", asMETHODPR(ASEmitter, AddInitiator_Position,(ASZone2D*,float),void), asCALL_THISCALL); assert( r >= 0 );
    //r = sm->m_Engine->RegisterObjectMethod("ParticleEffect", "void AddInitiatorSize(Zone &in z, float weight = 1.0)", asMETHODPR(ASEmitter, AddInitiator_Size,(ASZone2D*,float),void), asCALL_THISCALL); assert( r >= 0 );
    //r = sm->m_Engine->RegisterObjectMethod("ParticleEffect", "void AddInitiatorVelocity(Zone &in z, float weight = 1.0)", asMETHODPR(ASEmitter, AddInitiator_Velocity,(ASZone2D*,float),void), asCALL_THISCALL); assert( r >= 0 );
    //r = sm->m_Engine->RegisterObjectMethod("ParticleEffect", "void AddInitiatorColor(Zone &in z, float weight = 1.0)", asMETHODPR(ASEmitter, AddInitiator_Color,(ASZone2D*,float),void), asCALL_THISCALL); assert( r >= 0 );

    //add behaviors
    //r = sm->m_Engine->RegisterObjectMethod("ParticleEffect", "void AddBehaviorMove()", asMETHODPR(ASEmitter, AddBehavior_Move,(void),void), asCALL_THISCALL); assert( r >= 0 );


    //add Rendering option



    //add Instantiator
    r = sm->m_Engine->RegisterObjectMethod("ParticleEffect", "void AddInstantiatorBurst(int count)", asMETHODPR(ASEmitter,AddInstantiator_Burst,(int),void), asCALL_THISCALL); assert( r >= 0 );
    r = sm->m_Engine->RegisterObjectMethod("ParticleEffect", "void AddInstantiatorStream(float rate)", asMETHODPR(ASEmitter, AddInstantiator_Stream,(float),void), asCALL_THISCALL); assert( r >= 0 );
    r = sm->m_Engine->RegisterObjectMethod("ParticleEffect", "void AddInstantiatorNoisyStream(float rate, float noise_amp, float noise_period = 1.0)", asMETHODPR(ASEmitter, AddInstantiator_NoisyStream,(float,float,float),void), asCALL_THISCALL); assert( r >= 0 );
    r = sm->m_Engine->RegisterObjectMethod("ParticleEffect", "void AddInstantiatorDecliningNoisyStream(float rate, float noise_amp, float noise_period = 1.0, float decline_rate = 0.01)", asMETHODPR(ASEmitter, AddInstantiator_DecliningNoisyStream,(float,float,float,float),void), asCALL_THISCALL); assert( r >= 0 );

    r = sm->m_Engine->RegisterObjectMethod("ParticleEffect", "void AddInitiatorPosition(Zone &in z, float weight = 1.0)", asMETHODPR(ASEmitter,AddInitiator_Position,(ASZone2D *,float),void), asCALL_THISCALL); assert( r >= 0 );
    r = sm->m_Engine->RegisterObjectMethod("ParticleEffect", "void AddInitiatorSize(Zone &in z, float weight = 1.0)", asMETHODPR(ASEmitter, AddInitiator_Size,(ASZone2D *,float),void), asCALL_THISCALL); assert( r >= 0 );
    r = sm->m_Engine->RegisterObjectMethod("ParticleEffect", "void AddInitiatorRotation(Zone &in z, float weight = 1.0)", asMETHODPR(ASEmitter, AddInitiator_Rotation,(ASZone2D *,float),void), asCALL_THISCALL); assert( r >= 0 );
    r = sm->m_Engine->RegisterObjectMethod("ParticleEffect", "void AddInitiatorRotationVelocity(Zone &in z, float weight = 1.0)", asMETHODPR(ASEmitter, AddInitiator_RotationalVelocity,(ASZone2D *,float),void), asCALL_THISCALL); assert( r >= 0 );
    r = sm->m_Engine->RegisterObjectMethod("ParticleEffect", "void AddInitiatorVelocity(Zone &in z, float weight = 1.0)", asMETHODPR(ASEmitter, AddInitiator_Velocity,(ASZone2D *,float),void), asCALL_THISCALL); assert( r >= 0 );
    r = sm->m_Engine->RegisterObjectMethod("ParticleEffect", "void AddInitiatorColor(Zone &in z, float alpha_min = 0.0, float alpha_max = 1.0, float weight = 1.0)", asMETHODPR(ASEmitter, AddInitiator_Color,(ASZone2D *,float,float,float),void), asCALL_THISCALL); assert( r >= 0 );


    r = sm->m_Engine->RegisterObjectMethod("ParticleEffect", "void AddBehaviorMove()", asMETHODPR(ASEmitter, AddBehavior_Move,(void),void), asCALL_THISCALL); assert( r >= 0 );
    r = sm->m_Engine->RegisterObjectMethod("ParticleEffect", "void AddBehaviorBoundingBox(Region r)", asMETHODPR(ASEmitter, AddBehavior_BoundingBox,(BoundingBox),void), asCALL_THISCALL); assert( r >= 0 );


    r = sm->m_Engine->RegisterObjectMethod("ParticleEffect", "void SetRendererImage(string imagename,int fillstyle = 0)", asMETHODPR(ASEmitter, SetRenderer_Image,(QString name, int fillstyle ),void), asCALL_THISCALL); assert( r >= 0 );
    //r = sm->m_Engine->RegisterObjectSTDMethod("ParticleEffect", "void SetRendererImage(array<string> &in imagenames, array<float> &in probs, int fillstyle =0)", GetFuncConvert(CSetEmitterImageRenderer)); assert( r >= 0 );



    r = sm->m_Engine->RegisterGlobalFunction("UILayer AddViewLayer(ParticleEffect &in p, float time_scale = 1.0, bool auto_remove = false)", asMETHODPR( MapViewTool ,AddLayerFromScript,(ASEmitter*,float,bool),ASUILayer),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );


    //sm->m_Engine->RegisterGlobalFunction("UILayer AddParticleEffectLayer(string file, Region pos, GeoProjection &in p = GeoProjectionGeneric(), bool rel = false, int fillmode = 0)", asMETHODPR( MapViewTool ,AddImageLayer,(QString,BoundingBox,GeoProjection *,bool,int),ASUILayer),  asCALL_THISCALL_ASGLOBAL,this); assert( r >= 0 );














    sm->m_Engine->RegisterTypeDebugAction(sm->m_Engine->GetTypeIdByDecl("Map"),"View",[this](void* par){
        cTMap * m = (cTMap *)par;
        RasterDataProvider * rdp = new RasterDataProvider(QList<QList<cTMap*>>({{m->GetCopy()}}),false,true);

        if(rdp->Exists())
        {
            UIStreamRasterLayer * ret = new UIStreamRasterLayer(rdp,"DebugMap",m->AS_FileName.size() == 0? false : true,m->AS_FileName,!false);
            if(rdp->GetBandCount() > 1)
            {
                ret->SetStyle(GetStyleDefault(LISEM_STYLE_DEFAULT_MULTIRASTER),true);
            }else
            {
                ret->SetStyle(GetStyleDefault(LISEM_STYLE_DEFAULT_RASTERUI),true);
            }
            m_WorldWindow->AddUILayer(ret,true);

        }
    });


    sm->m_Engine->RegisterTypeDebugAction(sm->m_Engine->GetTypeIdByDecl("Map"),"Calculate Stats",[this](void* par){
        cTMap * m = (cTMap *)par;

        RasterBandStats stats = m->GetRasterBandStats(true);
        MatrixTable * t = new MatrixTable();
        t->SetSize(12,1);
        t->SetColumnTitle(0,QString("Value"));
        t->SetRowTitle(0,QString("Band"));t->SetValue(0,0,stats.band);
        t->SetRowTitle(1,QString("Rows"));t->SetValue(1,0,m->nrRows());
        t->SetRowTitle(2,QString("Cols"));t->SetValue(2,0,m->nrCols());
        t->SetRowTitle(3,QString("ULX"));t->SetValue(3,0,m->west());
        t->SetRowTitle(4,QString("ULY"));t->SetValue(4,0,m->north());
        t->SetRowTitle(5,QString("dx"));t->SetValue(5,0,m->cellSizeX());
        t->SetRowTitle(6,QString("dy"));t->SetValue(6,0,m->cellSizeY());
        t->SetRowTitle(7,QString("Mean"));t->SetValue(7,0,stats.mean);
        t->SetRowTitle(8,QString("min"));t->SetValue(8,0,stats.min);
        t->SetRowTitle(9,QString("max"));t->SetValue(9,0,stats.max);
        t->SetRowTitle(10,QString("stdev"));t->SetValue(10,0,stats.stdev);
        t->SetRowTitle(11,QString("MV"));t->SetValue(11,0,stats.n_mv);
        LayerInfoWidget *w = new LayerInfoWidget(t);
        w->show();

    });

    sm->m_Engine->RegisterTypeDebugAction(sm->m_Engine->GetTypeIdByDecl("Map"),"Save as",[this](void* par){
        cTMap * m = (cTMap *)par;

        QString path = QFileDialog::getOpenFileName(this,
                                            QString("Save map"),
                                            GetFIODir(LISEM_DIR_FIO_GEODATA),
                                            QString("*.tif;*.map;*.*"));

        if(!path.isEmpty())
        {
            SetFIODir(LISEM_DIR_FIO_GEODATA,QFileInfo(path).dir().absolutePath());

            AS_SaveMapToFileAbsolute(m,path);
        }
    });



    connect(this,&MapViewTool::int_on_create_plot,this,&MapViewTool::OnCreatePlot,Qt::ConnectionType::QueuedConnection);
    connect(this,&MapViewTool::int_on_add_plot,this,&MapViewTool::OnCreatePlotLayer,Qt::ConnectionType::QueuedConnection);
    connect(this,&MapViewTool::int_on_replace_plot,this,&MapViewTool::OnReplacePlotLayer,Qt::ConnectionType::QueuedConnection);
    connect(this,&MapViewTool::int_on_style_plot,this,&MapViewTool::OnSetPlotLayerStyle,Qt::ConnectionType::QueuedConnection);


}
