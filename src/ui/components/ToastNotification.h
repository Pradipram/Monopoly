#ifndef TOASTNOTIFICATION_H
#define TOASTNOTIFICATION_H

#include <QColor>
#include <QLabel>
#include <QWidget>

class ToastNotification : public QLabel
{
    Q_OBJECT
   public:
    static constexpr int kFadeInDurationMs = 300;
    static constexpr int kVisibleDurationMs = 2000;
    static constexpr int kFadeOutDurationMs = 500;

    static constexpr int totalDurationMs()
    {
        return kFadeInDurationMs + kVisibleDurationMs + kFadeOutDurationMs;
    }

    // Takes the text, the background color, and the parent window
    explicit ToastNotification(const QString& text, const QColor& bgColor,
                               QWidget* parent = nullptr);

   public slots:
    void showToast();

   signals:
    void toastClosed();
};

#endif  // TOASTNOTIFICATION_H