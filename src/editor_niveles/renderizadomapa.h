#ifndef RENDERIZADOMAPA_H
#define RENDERIZADOMAPA_H

#include <QPaintEvent>
#include <QPainter>
#include <QStandardItemModel>
#include <QWidget>
#include <optional>

#include "../common/map/map.h"

class MapRenderer : public QWidget {
    Q_OBJECT

   protected:
    std::optional<Map*> map;
    uint tile_size = 64;
    QPoint camera_reference = QPoint(0, 0);
    QPoint mouse_clicked_reference = QPoint(0, 0);
    bool moving_camera = false;
    QMap<IdTexture, QImage> tile_textures;
    QMap<IdTexture, QImage> background_textures;

   private:
   public:
    // explicit MapRenderer(QWidget* parent = nullptr, Map& map);
    explicit MapRenderer(QWidget* parent = nullptr);
    virtual ~MapRenderer();

    void addTileTextures(QMap<IdTexture, QImage> tile_textures);
    void addBackgroundTextures(QMap<IdTexture, QImage> background_textures);

    void paintEvent(QPaintEvent* event);
    void setMap(Map* map);

    void drawGrid(QPainter& painter);
    void drawBackground(QPainter& painter);

    void set_camera_reference(QPoint reference);
    void modify_camera_reference(QPoint delta);

   protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);
    void checkMapAvaible();

   signals:
};

#endif  // RENDERIZADOMAPA_H
