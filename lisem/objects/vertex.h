#ifndef GLVERTEX_H
#define GLVERTEX_H

#include "QObject"
#include "linear/lsm_vector3.h"
#include "linear/lsm_vector2.h"

struct Vertex
{
public:
  // Constructors
  Vertex();
  Vertex(const LSMVector3 &position);
  Vertex(const LSMVector3 &position, const LSMVector3 &color);
  Vertex(const LSMVector3 &position, const LSMVector3 &color, const LSMVector3 &normal);
  Vertex(const LSMVector3 &position, const LSMVector3 &color, const LSMVector3 &normal, const LSMVector2 &uv);

  // Accessors / Mutators
  const LSMVector3& position() const;
  const LSMVector3& color() const;
  const LSMVector3& normal() const;
  const LSMVector2& uv() const;
  const LSMVector3& tangent() const;
  const LSMVector3& bitangent() const;
  void setPosition(const LSMVector3& position);
  void setColor(const LSMVector3& color);
  void setNormal(const LSMVector3& normal);
  void setUV(const LSMVector2& uv);
  void setTangent(const LSMVector3& tangent);
  void setBiTangent(const LSMVector3& bitangent);

  // OpenGL Helpers
  static const int PositionTupleSize = 3;
  static const int ColorTupleSize = 3;
  static const int NormalTupleSize = 3;
  static const int UVTupleSize = 2;
  static const int TangentTupleSize = 3;
  static const int BiTangentTupleSize = 3;

  static int positionOffset();
  static int colorOffset();
  static int normalOffset();
  static int uvOffset();
  static int tangentOffset();
  static int bitangentOffset();

  static int stride();


  LSMVector3 m_position;
  LSMVector2 m_UV;
  LSMVector3 m_normal;
  LSMVector3 m_tangent;
  LSMVector3 m_bitangent;
  LSMVector3 m_color;

private:


};


// Constructors
inline Vertex::Vertex() {}
inline Vertex::Vertex(const LSMVector3 &position) : m_position(position) {}
inline Vertex::Vertex(const LSMVector3 &position, const LSMVector3 &color) : m_position(position), m_color(color) {}
inline Vertex::Vertex(const LSMVector3 &position, const LSMVector3 &color, const LSMVector3 &normal) : m_position(position), m_color(color), m_normal(normal) {}
inline Vertex::Vertex(const LSMVector3 &position, const LSMVector3 &color, const LSMVector3 &normal, const LSMVector2 &uv) : m_position(position), m_color(color), m_normal(normal), m_UV(uv) {}

// Accessors / Mutators
inline const LSMVector3& Vertex::position() const { return m_position; }
inline const LSMVector3& Vertex::color() const { return m_color; }
inline const LSMVector3& Vertex::normal() const { return m_normal; }
inline const LSMVector2& Vertex::uv() const { return m_UV; }
inline const LSMVector3& Vertex::tangent() const { return m_tangent; }
inline const LSMVector3& Vertex::bitangent() const { return m_bitangent; }
void inline Vertex::setPosition(const LSMVector3& position) { m_position = position; }
void inline Vertex::setColor(const LSMVector3& color) { m_color = color; }
void inline Vertex::setNormal(const LSMVector3& normal) { m_normal = normal; }
void inline Vertex::setUV(const LSMVector2& uv) { m_UV = uv; }
void inline Vertex::setTangent(const LSMVector3& tangent) { m_tangent = tangent; }
void inline Vertex::setBiTangent(const LSMVector3& bitangent) { m_bitangent = bitangent; }

// OpenGL Helpers
inline int Vertex::positionOffset() { return offsetof(Vertex, m_position); }
inline int Vertex::colorOffset() { return offsetof(Vertex, m_color); }
inline int Vertex::normalOffset() { return offsetof(Vertex, m_normal); }
inline int Vertex::uvOffset() { return offsetof(Vertex, m_UV); }
inline int Vertex::tangentOffset() { return offsetof(Vertex, m_tangent); }
inline int Vertex::bitangentOffset() { return offsetof(Vertex, m_bitangent); }
inline int Vertex::stride() { return sizeof(Vertex); }

#endif // GLVERTEX_H
