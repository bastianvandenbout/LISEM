#pragma once

#include <nodes/NodeData>
#include "memory"






/// The class can potentially incapsulate any user data which
/// need to be transferred within the Node Editor graph
class stringData : public QtNodes::NodeData
{
public:

  stringData()
  {}


  QtNodes::NodeDataType type() const override
  {
    return QtNodes::NodeDataType {"string",
                         "string"};
  }

private:

};


class TableData : public QtNodes::NodeData
{
public:

  TableData()
  {}


  QtNodes::NodeDataType type() const override
  {
    return QtNodes::NodeDataType {"Table",
                         "Table"};
  }

private:

};

class doubleData : public QtNodes::NodeData
{
public:

  doubleData()
  {}


  QtNodes::NodeDataType type() const override
  {
    return QtNodes::NodeDataType {"double",
                         "double"};
  }

private:

};

class floatData : public QtNodes::NodeData
{
public:

  floatData()
  {}


  QtNodes::NodeDataType type() const override
  {
    return QtNodes::NodeDataType {"float",
                         "float"};
  }

private:

};



class MapData : public QtNodes::NodeData
{
public:

  MapData()
  {}


  QtNodes::NodeDataType type() const override
  {
    return QtNodes::NodeDataType {"Map",
                         "Map"};
  }

private:

};


class ShapesData : public QtNodes::NodeData
{
public:

  ShapesData()
  {}


  QtNodes::NodeDataType type() const override
  {
    return QtNodes::NodeDataType {"Shapes",
                         "Shapes"};
  }

private:

};





class GeoProjectionData : public QtNodes::NodeData
{
public:

  GeoProjectionData()
  {}


  QtNodes::NodeDataType type() const override
  {
    return QtNodes::NodeDataType {"GeoProjection",
                         "GeoProjection"};
  }

private:

};


class boolData : public QtNodes::NodeData
{
public:

  boolData()
  {}


  QtNodes::NodeDataType type() const override
  {
    return QtNodes::NodeDataType {"bool",
                         "bool"};
  }

private:

};


class intData : public QtNodes::NodeData
{
public:

  intData()
  {}


  QtNodes::NodeDataType type() const override
  {
    return QtNodes::NodeDataType {"int",
                         "int"};
  }

private:

};



class PointCloudData : public QtNodes::NodeData
{
public:

  PointCloudData()
  {}


  QtNodes::NodeDataType type() const override
  {
    return QtNodes::NodeDataType {"PointCloud",
                         "PointCloud"};
  }

private:

};




class BoundingBoxData : public QtNodes::NodeData
{
public:

  BoundingBoxData()
  {}


  QtNodes::NodeDataType type() const override
  {
    return QtNodes::NodeDataType {"BoundingBox",
                         "BoundingBox"};
  }

private:

};

class PointData : public QtNodes::NodeData
{
public:

  PointData()
  {}


  QtNodes::NodeDataType type() const override
  {
    return QtNodes::NodeDataType {"Point",
                         "Point"};
  }

private:

};



class unknownData : public QtNodes::NodeData
{
public:

  unknownData()
  {}


  QtNodes::NodeDataType type() const override
  {
    return QtNodes::NodeDataType {"unknown",
                         "unknown"};
  }

private:

};


inline static QtNodes::NodeDataType GetDataFromTypeName(QString name)
{

    if(name == "Map")
    {
        return MapData().type();
    }
    if(name == "Shapes")
    {
        return ShapesData().type();
    }
    if(name == "Table")
    {
        return TableData().type();
    }
    if(name == "PointCloud")
    {
        return PointCloudData().type();
    }
    if(name == "BoundingBox")
    {
        return BoundingBoxData().type();
    }
    if(name == "string")
    {
        return stringData().type();
    }
    if(name == "bool")
    {
        return boolData().type();
    }
    if(name == "double")
    {
        return doubleData().type();
    }
    if(name == "float")
    {
        return floatData().type();
    }
    if(name == "int")
    {
        return intData().type();
    }
    if(name == "Point")
    {
        return PointData().type();
    }

    return unknownData().type();
}



class FloatToIntConverter
{

public:

  inline std::shared_ptr<QtNodes::NodeData>
  operator()(std::shared_ptr<QtNodes::NodeData> data)
  {
        return std::make_shared<intData>();
  }
};


class DoubleToIntConverter
{

public:

  inline std::shared_ptr<QtNodes::NodeData>
  operator()(std::shared_ptr<QtNodes::NodeData> data)
  {
        return std::make_shared<intData>();
  }

};

class BoolToIntConverter
{

public:

  inline std::shared_ptr<QtNodes::NodeData>
  operator()(std::shared_ptr<QtNodes::NodeData> data)
  {
        return std::make_shared<intData>();
  }

};

class IntToDoubleConverter
{

public:

  inline std::shared_ptr<QtNodes::NodeData>
  operator()(std::shared_ptr<QtNodes::NodeData> data)
  {
        return std::make_shared<doubleData>();
  }

};

class FloatToDoubleConverter
{

public:

  inline std::shared_ptr<QtNodes::NodeData>
  operator()(std::shared_ptr<QtNodes::NodeData> data)
  {
        return std::make_shared<doubleData>();
  }

};


class IntToFloatConverter
{

public:

  inline std::shared_ptr<QtNodes::NodeData>
  operator()(std::shared_ptr<QtNodes::NodeData> data)
  {
        return std::make_shared<floatData>();
  }

};



class DoubleToFloatConverter
{

public:

  inline std::shared_ptr<QtNodes::NodeData>
  operator()(std::shared_ptr<QtNodes::NodeData> data)
  {
        return std::make_shared<floatData>();
  }

};
