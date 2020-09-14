#ifndef NAVLISTVIEW_H
#define NAVLISTVIEW_H


#include <QStyledItemDelegate>
#include <QAbstractListModel>
#include <QListView>
#include <vector>

class NavListView;

class NavDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    NavDelegate(QObject *parent);
    ~NavDelegate();

protected:
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const ;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    NavListView *nav;
    QFont iconFont;
};


class NavModel : public QAbstractListModel
{
    Q_OBJECT
public:
    NavModel(QObject *parent);
    ~NavModel();

public:
    struct TreeNode {
        QString label;
        int level;
        bool collapse;
        bool theLast;
        QString info;
        std::list<TreeNode *> children;
    };

    struct ListNode {
        QString label;
        TreeNode *treeNode;
    };

protected:
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

private:
    std::vector<TreeNode *> treeNode;
    std::vector<ListNode> listNode;

public slots:    
    void setData(const QStringList &listItem);
    void collapse(const QModelIndex &index);

private:
    void refreshList();
};

#ifdef quc
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
#include <QtDesigner/QDesignerExportWidget>
#else
#include <QtUiPlugin/QDesignerExportWidget>
#endif

class QDESIGNER_WIDGET_EXPORT NavListView : public QListView
#else
class NavListView : public QListView
#endif

{
    Q_OBJECT
    Q_ENUMS(IcoStyle)

    Q_PROPERTY(bool infoVisible READ getInfoVisible WRITE setInfoVisible)
    Q_PROPERTY(bool lineVisible READ getLineVisible WRITE setLineVisible)
    Q_PROPERTY(bool icoColorBg READ getIcoColorBg WRITE setIcoColorBg)
    Q_PROPERTY(IcoStyle icoStyle READ getIcoStyle WRITE setIcoStyle)

    Q_PROPERTY(QColor colorLine READ getColorLine WRITE setColorLine)
    Q_PROPERTY(QColor colorBgNormal READ getColorBgNormal WRITE setColorBgNormal)
    Q_PROPERTY(QColor colorBgSelected READ getColorBgSelected WRITE setColorBgSelected)
    Q_PROPERTY(QColor colorBgHover READ getColorBgHover WRITE setColorBgHover)

    Q_PROPERTY(QColor colorTextNormal READ getColorTextNormal WRITE setColorTextNormal)
    Q_PROPERTY(QColor colorTextSelected READ getColorTextSelected WRITE setColorTextSelected)
    Q_PROPERTY(QColor colorTextHover READ getColorTextHover WRITE setColorTextHover)

public:
    enum IcoStyle {
        IcoStyle_Cross = 0,     //十字形状
        IcoStyle_Triangle = 1   //三角形状
    };

    NavListView(QWidget *parent = 0);
    ~NavListView();

private:
    NavModel *model;
    NavDelegate *delegate;

    bool infoVisible;               //是否显示提示信息
    bool lineVisible;               //是否显示分割线条
    bool icoColorBg;                //伸缩图片是否使用颜色
    IcoStyle icoStyle;              //图标样式

    QColor colorLine;               //线条颜色
    QColor colorBgNormal;           //正常背景色
    QColor colorBgSelected;         //选中背景色
    QColor colorBgHover;            //悬停背景色

    QColor colorTextNormal;         //正常文字颜色
    QColor colorTextSelected;       //选中文字颜色
    QColor colorTextHover;          //悬停文字颜色

public:
    bool getInfoVisible()           const;
    bool getLineVisible()           const;
    bool getIcoColorBg()            const;
    IcoStyle getIcoStyle()          const;

    QColor getColorLine()           const;
    QColor getColorBgNormal()       const;
    QColor getColorBgSelected()     const;
    QColor getColorBgHover()        const;

    QColor getColorTextNormal()     const;
    QColor getColorTextSelected()   const;
    QColor getColorTextHover()      const;

    QSize sizeHint()                const;
    QSize minimumSizeHint()         const;

public Q_SLOTS:    
    //设置数据集合
    void setData(const QStringList &listItem);

    //设置是否显示提示信息
    void setInfoVisible(bool infoVisible);

    //设置是否显示间隔线条
    void setLineVisible(bool lineVisible);

    //设置伸缩图片是否采用背景色
    void setIcoColorBg(bool icoColorBg);

    //设置伸缩图片样式
    void setIcoStyle(IcoStyle icoStyle);

    //设置各种前景色背景色选中色
    void setColorLine(const QColor &colorLine);

    void setColorBgNormal(const QColor &colorBgNormal);
    void setColorBgSelected(const QColor &colorBgSelected);
    void setColorBgHover(const QColor &colorBgHover);

    void setColorTextNormal(const QColor &colorTextNormal);
    void setColorTextSelected(const QColor &colorTextSelected);
    void setColorTextHover(const QColor &colorTextHover);
};

#endif // NAVLISTVIEW_H
