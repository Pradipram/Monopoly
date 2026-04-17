#ifndef PLAYERINFOWIDGET_H
#define PLAYERINFOWIDGET_H

#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

class PlayerInfoWidget : public QWidget
{
    Q_OBJECT
   public:
    explicit PlayerInfoWidget(int playerId, QColor playerColor, QString playerName,
                              QWidget* parent = nullptr);

    // Functions to update the UI later
    void updateCash(int newAmount);
    void updatePropertiesInfo(int propertyCount);

   private:
    QLabel* m_nameLabel;
    QLabel* m_cashLabel;
    QLabel* m_propertiesLabel;
};

#endif  // PLAYERINFOWIDGET_H