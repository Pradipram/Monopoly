#ifndef PLAYERINFOWIDGET_H
#define PLAYERINFOWIDGET_H

#include <QHBoxLayout>
#include <QLabel>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QVBoxLayout>
#include <QVector>
#include <QWidget>

class Space;

class PlayerInfoWidget : public QWidget
{
    Q_OBJECT
   public:
    explicit PlayerInfoWidget(int playerId, int cash, QColor playerColor, const QString& playerName,
                              QWidget* parent = nullptr);

    // Functions to update the UI later
    void updateCash(int newAmount);
    void updateNetWorth(int netWorth);
    void setOwnedProperties(const QVector<Space*>& spaces);
    void setActive(bool isActive);

   private:
    void showOwnedPropertiesDialog();

    QLabel* m_nameLabel;
    QLabel* m_cashLabel;
    QLabel* m_netWorthLabel;
    QPushButton* m_propertiesButton;
    QColor m_playerColor;
    QVector<Space*> m_ownedSpaces;
    int m_currentCash = 0;  // Track current cash for animation
};

#endif  // PLAYERINFOWIDGET_H