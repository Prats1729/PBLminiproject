// ui_main.cpp — Qt5 Widgets visualization for Smart Disaster Response
// Compile:
//   moc $(pkg-config --cflags Qt5Widgets) ui_main.cpp -o ui_main.moc
//   g++ -std=c++17 -fPIC ui_main.cpp Data.cpp Graph.cpp bfs.cpp dijkstra.cpp knapsack.cpp \
//       $(pkg-config --cflags --libs Qt5Widgets) -o disaster_ui
//   ./disaster_ui

#include <QApplication>
#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsProxyWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <QWidget>
#include <QFont>
#include <QPen>
#include <QBrush>
#include <QColor>
#include <QFrame>
#include <QRadialGradient>
#include <QLinearGradient>
#include <QPainter>
#include <QTimer>
#include <QPropertyAnimation>
#include <QScrollBar>
#include <QFontDatabase>
#include <QDialog>
#include <QHeaderView>
#include <QTableWidget>
#include <QCheckBox>
#include <QSpinBox>
#include <QScrollArea>
#include <QToolTip>

#include "greedy.h"

#include <cmath>
#include <limits>
#include <sstream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>

#include "Data.h"
#include "Graph.h"
#include "bfs.h"
#include "dijkstra.h"
#include "knapsack.h"

// ── Constants ──────────────────────────────────────────────
static constexpr double NODE_RADIUS = 26.0;
static constexpr double SCENE_W     = 1200.0;
static constexpr double SCENE_H     = 800.0;

// ── Color palette (dark, modern) ───────────────────────────
static const QColor kBgDark        (18,  18,  28);
static const QColor kBgPanel       (26,  28,  44);
static const QColor kBgCard        (34,  38,  58);
static const QColor kNodeDefault   (88, 101, 242);   // Discord-ish blue
static const QColor kNodeDisaster  (237, 66,  69);    // red
static const QColor kNodeResponder (87, 242, 135);    // green
static const QColor kNodeNeutral   (110, 120, 160);   // muted
static const QColor kNodeHighlight (250, 168,  26);   // amber
static const QColor kBfsColor      (59, 165, 232);    // sky blue
static const QColor kPathColor     (87, 242, 135);    // green
static const QColor kEdgeDefault   (55,  60,  85);
static const QColor kEdgeHighlight (250, 168,  26);
static const QColor kTextPrimary   (230, 233, 245);
static const QColor kTextSecondary (140, 148, 178);
static const QColor kTextMuted     (90,  98, 128);
static const QColor kAccentPurple  (138, 79, 255);
static const QColor kAccentCyan    (0, 209, 209);
static const QColor kBorderSubtle  (50,  55,  80);
static const QColor kWeightBadge   (45,  48,  72);

// ── Hand-placed node positions for a realistic map feel ────
static std::vector<QPointF> getNodePositions() {
    //  Map layout (roughly):
    //
    //      [7 IT Park]────────────[0 Hospital]────[1 Fire Stn]
    //          │                       │                │
    //          │               [2 Central Market]       │
    //          │              ╱        │        ╲       │
    //     [6 Industrial]    ╱         │         [3 Railway]
    //          │          ╱           │                │
    //
    return {
        {130, 80},    // 0
        {325, 105},   // 1
        {585, 90},    // 2
        {845, 115},   // 3
        {1050, 260},  // 4
        {105, 260},   // 5
        {285, 285},   // 6
        {495, 325},   // 7
        {715, 310},   // 8
        {935, 365},   // 9
        {155, 470},   // 10
        {365, 495},   // 11
        {545, 455},   // 12
        {780, 505},   // 13
        {990, 545},   // 14
        {195, 675},   // 15
        {415, 650},   // 16
        {650, 690},   // 17
        {885, 700},   // 18
        {1040, 715},  // 19
    };
}

// ── Determine node role for coloring ───────────────────────
enum class NodeRole { Neutral, Disaster, Responder };

static std::vector<NodeRole> classifyNodes(const InputData& data) {
    std::vector<NodeRole> roles(data.numNodes, NodeRole::Neutral);
    for (const auto& s : data.sites)      roles[s.nodeId] = NodeRole::Disaster;
    for (const auto& r : data.responders) roles[r.nodeId] = NodeRole::Responder;
    return roles;
}

static QColor roleColor(NodeRole r) {
    switch (r) {
        case NodeRole::Disaster:  return kNodeDisaster;
        case NodeRole::Responder: return kNodeResponder;
        default:                  return kNodeNeutral;
    }
}

// ── Glow ellipse (node with gradient + shadow) ─────────────
class GlowNode : public QGraphicsEllipseItem {
public:
    GlowNode(double x, double y, double r, QColor base)
        : QGraphicsEllipseItem(x - r, y - r, r * 2, r * 2),
          center_(x, y), baseColor_(base), radius_(r) {
        applyStyle(base);
    }

    void setHighlight(const QColor& c) {
        applyStyle(c);
    }

    void resetStyle() {
        applyStyle(baseColor_);
    }

    QPointF center() const { return center_; }

private:
    void applyStyle(const QColor& c) {
        QRadialGradient grad(center_, radius_);
        grad.setColorAt(0.0, c.lighter(150));
        grad.setColorAt(0.7, c);
        grad.setColorAt(1.0, c.darker(130));
        setBrush(QBrush(grad));
        setPen(QPen(c.lighter(120), 2.0));

        auto* shadow = new QGraphicsDropShadowEffect();
        shadow->setBlurRadius(18);
        shadow->setColor(QColor(c.red(), c.green(), c.blue(), 100));
        shadow->setOffset(0, 0);
        setGraphicsEffect(shadow);
    }

    QPointF center_;
    QColor  baseColor_;
    double  radius_;
};

// ── Styled button helper ───────────────────────────────────
static const QString kBtnStyle = R"(
    QPushButton {
        background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
            stop:0 %1, stop:1 %2);
        color: %3;
        border: 1px solid %4;
        border-radius: 10px;
        font-size: 13px;
        font-weight: 600;
        padding: 10px 14px;
        text-align: left;
    }
    QPushButton:hover {
        background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
            stop:0 %5, stop:1 %1);
        border: 1px solid %6;
    }
    QPushButton:pressed {
        background: %7;
    }
)";

static QPushButton* makeButton(const QString& text, const QColor& accent,
                               QWidget* parent) {
    auto* btn = new QPushButton(text, parent);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setMinimumHeight(44);

    QString style = kBtnStyle
        .arg(kBgCard.name())
        .arg(kBgPanel.name())
        .arg(kTextPrimary.name())
        .arg(kBorderSubtle.name())
        .arg(accent.darker(140).name())
        .arg(accent.name())
        .arg(accent.darker(200).name());

    btn->setStyleSheet(style);
    return btn;
}

// ── Main Window ────────────────────────────────────────────
class DisasterUI : public QMainWindow {
    Q_OBJECT
public:
    DisasterUI() {
        setWindowTitle("Smart Disaster Response — Control Center");
        resize(1220, 720);
        setMinimumSize(1000, 600);
        setStyleSheet(QString("QMainWindow { background: %1; }").arg(kBgDark.name()));

        auto* central = new QWidget(this);
        setCentralWidget(central);

        auto* root = new QHBoxLayout(central);
        root->setContentsMargins(12, 12, 12, 12);
        root->setSpacing(12);

        // ── Left panel ─────────────────────────────────────
        auto* panel = new QFrame(this);
        panel->setFixedWidth(280);
        panel->setStyleSheet(QString(
            "QFrame#sidePanel { background: %1; border-radius: 14px;"
            " border: 1px solid %2; }")
                .arg(kBgPanel.name(), kBorderSubtle.name()));
        panel->setObjectName("sidePanel");

        auto* pLay = new QVBoxLayout(panel);
        pLay->setContentsMargins(18, 22, 18, 18);
        pLay->setSpacing(8);

        // Title
        auto* logo = new QLabel("🚨", panel);
        logo->setAlignment(Qt::AlignCenter);
        logo->setStyleSheet("font-size: 36px;");
        pLay->addWidget(logo);

        auto* title = new QLabel("Disaster Response\nControl Center", panel);
        title->setAlignment(Qt::AlignCenter);
        title->setStyleSheet(QString(
            "QLabel { color: %1; font-size: 17px; font-weight: 700;"
            " letter-spacing: 0.5px; }").arg(kTextPrimary.name()));
        pLay->addWidget(title);

        auto* subtitle = new QLabel("Graph-based Optimization", panel);
        subtitle->setAlignment(Qt::AlignCenter);
        subtitle->setStyleSheet(QString(
            "QLabel { color: %1; font-size: 11px; margin-bottom: 8px; }")
                .arg(kTextMuted.name()));
        pLay->addWidget(subtitle);

        // Separator
        auto* sep = new QFrame(panel);
        sep->setFrameShape(QFrame::HLine);
        sep->setFixedHeight(1);
        sep->setStyleSheet(QString("background: %1;").arg(kBorderSubtle.name()));
        pLay->addWidget(sep);
        pLay->addSpacing(4);

        // Section label
        auto addSection = [&](const QString& text) {
            auto* lbl = new QLabel(text, panel);
            lbl->setStyleSheet(QString(
                "QLabel { color: %1; font-size: 10px; font-weight: 700;"
                " text-transform: uppercase; letter-spacing: 1px; margin-top: 6px; }")
                    .arg(kTextMuted.name()));
            pLay->addWidget(lbl);
        };

        addSection("DATA");
        btnLoad_        = makeButton("▶  Load Scenario", kAccentCyan, panel);
        btnConfigSites_ = makeButton("⚙  Configure Disaster Sites", kNodeDisaster, panel);
        btnConfigSites_->setEnabled(false);
        pLay->addWidget(btnLoad_);
        pLay->addWidget(btnConfigSites_);

        addSection("ALGORITHMS");
        btnGreedy_   = makeButton("🏆  Greedy — Triage Rank", kNodeDisaster, panel);
        btnBfs_      = makeButton("🔍  BFS — Reachable Responders", kBfsColor, panel);
        btnDijkstra_ = makeButton("📍  Dijkstra — Shortest Path", kPathColor, panel);
        btnKnapsack_ = makeButton("🎒  Knapsack — Resource Alloc", kAccentPurple, panel);
        btnViewAlloc_ = makeButton("📋  View Allocation Details", kTextSecondary, panel);
        btnViewAlloc_->setEnabled(false);

        pLay->addWidget(btnGreedy_);
        pLay->addWidget(btnBfs_);
        pLay->addWidget(btnDijkstra_);
        pLay->addWidget(btnKnapsack_);
        pLay->addWidget(btnViewAlloc_);

        addSection("QUICK ACTIONS");
        btnRunAll_ = makeButton("⚡  Run Full Demo", kNodeHighlight, panel);
        btnReset_  = makeButton("↺   Reset View", kTextSecondary, panel);
        btnReport_ = makeButton("📄  Generate Status Report", kAccentCyan, panel);
        pLay->addWidget(btnRunAll_);
        pLay->addWidget(btnReset_);
        pLay->addWidget(btnReport_);

        pLay->addStretch();

        // Legend
        addSection("LEGEND");
        auto addLegendItem = [&](const QString& text, const QColor& color) {
            auto* row = new QHBoxLayout();
            auto* dot = new QFrame(panel);
            dot->setFixedSize(12, 12);
            dot->setStyleSheet(QString(
                "background: %1; border-radius: 6px;").arg(color.name()));
            auto* lbl = new QLabel(text, panel);
            lbl->setStyleSheet(QString("color: %1; font-size: 11px;")
                .arg(kTextSecondary.name()));
            row->addWidget(dot);
            row->addWidget(lbl);
            row->addStretch();
            pLay->addLayout(row);
        };
        addLegendItem("Disaster Site", kNodeDisaster);
        addLegendItem("Responder", kNodeResponder);
        addLegendItem("Neutral", kNodeNeutral);
        addLegendItem("BFS Reached", kBfsColor);
        addLegendItem("Shortest Path", kPathColor);

        root->addWidget(panel);

        // ── Right column (graph + log) ─────────────────────
        auto* rightCol = new QVBoxLayout();
        rightCol->setSpacing(10);

        scene_ = new QGraphicsScene(0, 0, SCENE_W, SCENE_H, this);
        scene_->setBackgroundBrush(kBgDark);

        view_ = new QGraphicsView(scene_, this);
        view_->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
        view_->setStyleSheet(QString(
            "QGraphicsView { border: 1px solid %1; border-radius: 12px;"
            " background: %2; }")
                .arg(kBorderSubtle.name(), kBgDark.name()));
        view_->setDragMode(QGraphicsView::ScrollHandDrag);
        view_->setMinimumHeight(380);
        rightCol->addWidget(view_, 3);

        // Results log
        results_ = new QTextEdit(this);
        results_->setReadOnly(true);
        results_->setStyleSheet(QString(
            "QTextEdit { background: %1; color: %2; border: 1px solid %3;"
            " border-radius: 10px; font-family: 'JetBrains Mono', 'Fira Code',"
            " 'Cascadia Code', monospace; font-size: 15px; padding: 12px;"
            " line-height: 1.5; selection-background-color: %4; }")
                .arg(kBgPanel.name(), kTextPrimary.name(),
                     kBorderSubtle.name(), kAccentPurple.darker(160).name()));
        results_->setMaximumHeight(220);
        rightCol->addWidget(results_, 1);

        root->addLayout(rightCol, 1);

        // ── Connections ────────────────────────────────────
        connect(btnLoad_,       &QPushButton::clicked, this, &DisasterUI::onLoad);
        connect(btnConfigSites_,&QPushButton::clicked, this, &DisasterUI::onConfigSites);
        connect(btnGreedy_,     &QPushButton::clicked, this, &DisasterUI::onGreedy);
        connect(btnBfs_,        &QPushButton::clicked, this, &DisasterUI::onBfsDefault);
        connect(btnDijkstra_,   &QPushButton::clicked, this, &DisasterUI::onDijkstraDefault);
        connect(btnKnapsack_,   &QPushButton::clicked, this, &DisasterUI::onKnapsack);
        connect(btnViewAlloc_,  &QPushButton::clicked, this, &DisasterUI::onViewAllocation);
        connect(btnRunAll_,     &QPushButton::clicked, this, &DisasterUI::onRunAll);
        connect(btnReset_,      &QPushButton::clicked, this, &DisasterUI::onReset);
        connect(btnReport_,     &QPushButton::clicked, this, &DisasterUI::onReport);

        log("🟢 <b>Control Center online.</b>  Click <b>Load Scenario</b> to begin.");
    }

private slots:
    void onLoad() {
        data_  = loadHardcodedData();
        graph_ = std::make_unique<Graph>(data_.numNodes);
        graph_->setNodeNames(data_.nodeNames);
        for (const auto& [u, v, w] : data_.edges) {
            graph_->addEdge(u, v, w);
        }
        roles_     = classifyNodes(data_);
        positions_ = getNodePositions();
        loaded_    = true;
        drawGraph();
        btnConfigSites_->setEnabled(true);

        logSiteSummary();
    }

    void onBfsDefault() { animateBfs(nullptr); }
    void onDijkstraDefault() { animateDijkstra(nullptr); }

    void animateBfs(std::function<void()> onDone) {
        if (!ensureLoaded()) { if (onDone) onDone(); return; }
        if (isAnimating_) return;

        isAnimating_ = true;
        setControlsEnabled(false);
        resetHighlights();

        std::vector<int> responderIds;
        for (const auto& r : data_.responders) responderIds.push_back(r.nodeId);

        int origin = getHighestSeverityOrigin();
        auto reachable = bfsAllResponders(*graph_, origin, responderIds);
        highlightNode(origin, kNodeDisaster);

        log("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
        log("🔍 <b>BFS (O(V+E))</b> from <b>" + data_.nodeNames[origin] + "</b>:");

        auto* stepTimer = new QTimer(this);
        connect(stepTimer, &QTimer::timeout, this, [this, reachable, origin, stepTimer, onDone, idx = 0]() mutable {
            if (idx < static_cast<int>(reachable.size())) {
                int id = reachable[idx];
                highlightNode(id, kBfsColor);
                log("   ✓ " + data_.nodeNames[id]);
                idx++;
            } else {
                log("   Found <b>" + std::to_string(reachable.size()) + "</b> reachable responder(s).");
                stepTimer->stop();
                stepTimer->deleteLater();
                isAnimating_ = false;
                setControlsEnabled(true);
                if (onDone) onDone();
            }
        });
        stepTimer->start(400);
    }

    void animateDijkstra(std::function<void()> onDone) {
        if (!ensureLoaded()) { if (onDone) onDone(); return; }
        if (isAnimating_) return;

        isAnimating_ = true;
        setControlsEnabled(false);
        resetHighlights();

        std::vector<int> responderIds;
        for (const auto& r : data_.responders) responderIds.push_back(r.nodeId);

        int origin = getHighestSeverityOrigin();
        Dijkstra dijk(*graph_);
        dijk.computeShortestPaths(origin);

        int closest = -1, minDist = std::numeric_limits<int>::max();
        for (int rid : responderIds) {
            int d = dijk.getDistance(rid);
            if (d < minDist) { minDist = d; closest = rid; }
        }

        if (closest == -1) { 
            log("⚠️  No responder reachable."); 
            isAnimating_ = false;
            setControlsEnabled(true);
            if (onDone) onDone(); 
            return; 
        }

        auto path = dijk.getPath(closest);
        log("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
        log("📍 <b>Dijkstra (O((V+E) log V))</b> — Nearest responder: <b>" + data_.nodeNames[closest] + "</b> (" + std::to_string(minDist) + " min)");
        highlightNode(origin, kNodeDisaster);

        auto* stepTimer = new QTimer(this);
        connect(stepTimer, &QTimer::timeout, this, [this, path, closest, origin, responderIds, dijk, stepTimer, onDone, idx = 1]() mutable {
            if (idx < static_cast<int>(path.size())) {
                highlightEdge(path[idx - 1], path[idx], kPathColor, 4.5);
                highlightNode(path[idx], kPathColor);
                log("   → " + data_.nodeNames[path[idx]]);
                idx++;
            } else {
                highlightNode(closest, kNodeHighlight);
                log("   🏁 Route complete.");
                log("   <i>All distances from " + data_.nodeNames[origin] + ":</i>");
                for (int rid : responderIds) {
                    int d = dijk.getDistance(rid);
                    std::string dist = (d == std::numeric_limits<int>::max()) ? "∞" : std::to_string(d);
                    log("      " + data_.nodeNames[rid] + ": " + dist + " min"
                        + (rid == closest ? "  ← <b>nearest</b>" : ""));
                }
                stepTimer->stop();
                stepTimer->deleteLater();
                isAnimating_ = false;
                setControlsEnabled(true);
                if (onDone) onDone();
            }
        });
        stepTimer->start(400);
    }

    void onKnapsack() {
        if (!ensureLoaded()) return;

        KnapsackSolver solver(data_);
        lastAllocation_ = solver.allocateResources(0);

        int totalW = 0, totalV = 0;
        for (int id : lastAllocation_) {
            totalW += data_.resources[id].weight;
            totalV += data_.resources[id].value;
        }

        log("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
        log("🎒 <b>Knapsack (O(n·W))</b> — capacity: <b>" + std::to_string(data_.knapsackCapacity)
            + "</b> units");
        log("   Selected <b>" + std::to_string(lastAllocation_.size())
            + "</b> items  |  weight: <b>" + std::to_string(totalW)
            + "/" + std::to_string(data_.knapsackCapacity)
            + "</b>  |  value: <b>" + std::to_string(totalV) + "</b>");
        log("   Click <b>View Allocation Details</b> to see the full list.");

        btnViewAlloc_->setEnabled(true);
    }

    void onViewAllocation() {
        if (lastAllocation_.empty()) return;

        auto* dlg = new QDialog(this);
        dlg->setWindowTitle("Resource Allocation Details");
        dlg->resize(520, 400);
        dlg->setStyleSheet(QString(
            "QDialog { background: %1; }"
            "QLabel  { color: %2; }"
            "QTableWidget { background: %3; color: %4;"
            "  border: 1px solid %5; border-radius: 8px;"
            "  font-size: 14px; gridline-color: %5; }"
            "QTableWidget::item { padding: 6px; }"
            "QHeaderView::section { background: %6; color: %7;"
            "  font-weight: 700; font-size: 13px; padding: 8px;"
            "  border: none; border-bottom: 2px solid %8; }")
                .arg(kBgDark.name(), kTextPrimary.name(),
                     kBgPanel.name(), kTextPrimary.name(),
                     kBorderSubtle.name(), kBgCard.name(),
                     kTextPrimary.name(), kAccentPurple.name()));

        auto* lay = new QVBoxLayout(dlg);
        lay->setContentsMargins(20, 20, 20, 20);
        lay->setSpacing(12);

        auto* header = new QLabel("🎒 Optimal Resource Allocation", dlg);
        header->setStyleSheet(QString(
            "font-size: 18px; font-weight: 700; color: %1;").arg(kTextPrimary.name()));
        lay->addWidget(header);

        auto* capLbl = new QLabel(
            QString("Vehicle capacity: %1 units").arg(data_.knapsackCapacity), dlg);
        capLbl->setStyleSheet(QString("color: %1; font-size: 13px;").arg(kTextSecondary.name()));
        lay->addWidget(capLbl);

        // Table
        int rows = static_cast<int>(lastAllocation_.size());
        auto* table = new QTableWidget(rows, 3, dlg);
        table->setHorizontalHeaderLabels({"Resource", "Weight", "Value"});
        table->horizontalHeader()->setStretchLastSection(true);
        table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
        table->verticalHeader()->setVisible(false);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table->setSelectionMode(QAbstractItemView::NoSelection);
        table->setShowGrid(false);
        table->setAlternatingRowColors(true);
        table->setStyleSheet(table->styleSheet()
            + QString("QTableWidget::item:alternate { background: %1; }")
                  .arg(kBgCard.name()));

        int totalW = 0, totalV = 0;
        for (int row = 0; row < rows; ++row) {
            const auto& res = data_.resources[lastAllocation_[row]];
            table->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(res.name)));
            table->setItem(row, 1, new QTableWidgetItem(QString::number(res.weight)));
            table->setItem(row, 2, new QTableWidgetItem(QString::number(res.value)));
            totalW += res.weight;
            totalV += res.value;
        }
        lay->addWidget(table);

        // Totals row
        auto* totals = new QLabel(
            QString("<b>Total:  weight = %1 / %2  |  value = %3</b>")
                .arg(totalW).arg(data_.knapsackCapacity).arg(totalV), dlg);
        totals->setStyleSheet(QString(
            "color: %1; font-size: 15px; padding: 8px 0;").arg(kAccentPurple.name()));
        lay->addWidget(totals);

        // Close button
        auto* closeBtn = new QPushButton("Close", dlg);
        closeBtn->setStyleSheet(QString(
            "QPushButton { background: %1; color: %2; border: 1px solid %3;"
            " border-radius: 8px; padding: 10px 24px; font-size: 14px; font-weight: 600; }"
            "QPushButton:hover { background: %4; }")
                .arg(kBgCard.name(), kTextPrimary.name(),
                     kBorderSubtle.name(), kAccentPurple.darker(160).name()));
        closeBtn->setCursor(Qt::PointingHandCursor);
        connect(closeBtn, &QPushButton::clicked, dlg, &QDialog::accept);
        lay->addWidget(closeBtn, 0, Qt::AlignRight);

        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->exec();
    }

    void onConfigSites() {
        if (!ensureLoaded()) return;

        auto* dlg = new QDialog(this);
        dlg->setWindowTitle("Configure Disaster Sites");
        dlg->resize(620, 520);
        dlg->setStyleSheet(QString(
            "QDialog { background: %1; }"
            "QLabel  { color: %2; }"
            "QCheckBox { color: %3; font-size: 13px; spacing: 8px; }"
            "QCheckBox::indicator { width: 18px; height: 18px;"
            "  border: 2px solid %4; border-radius: 4px; background: %5; }"
            "QCheckBox::indicator:checked { background: %6; border-color: %6; }"
            "QSpinBox { background: %7; color: %8; border: 1px solid %9;"
            "  border-radius: 6px; padding: 4px 8px; font-size: 13px;"
            "  min-width: 60px; }")
                .arg(kBgDark.name(), kTextPrimary.name(), kTextPrimary.name(),
                     kBorderSubtle.name(), kBgCard.name(),
                     kNodeDisaster.name(), kBgCard.name(),
                     kTextPrimary.name(), kBorderSubtle.name()));

        auto* lay = new QVBoxLayout(dlg);
        lay->setContentsMargins(24, 24, 24, 24);
        lay->setSpacing(14);

        auto* header = new QLabel("🔴 Configure Disaster Sites", dlg);
        header->setStyleSheet(QString(
            "font-size: 20px; font-weight: 700; color: %1;").arg(kTextPrimary.name()));
        lay->addWidget(header);

        auto* hint = new QLabel(
            "Check a location to mark it as a disaster site. Set severity (1–10) and population.", dlg);
        hint->setWordWrap(true);
        hint->setStyleSheet(QString("color: %1; font-size: 12px; margin-bottom: 4px;").arg(kTextSecondary.name()));
        lay->addWidget(hint);

        // Build a set of current disaster-site node IDs for pre-filling
        std::unordered_map<int, DisasterSite*> siteByNode;
        for (auto& s : data_.sites) siteByNode[s.nodeId] = &s;

        // Per-node row widgets
        struct RowWidgets {
            QCheckBox* check;
            QSpinBox*  severity;
            QSpinBox*  population;
        };
        std::vector<RowWidgets> rows(data_.numNodes);

        // Scrollable area with all nodes
        auto* scrollWidget = new QWidget(dlg);
        auto* grid = new QVBoxLayout(scrollWidget);
        grid->setSpacing(6);

        for (int i = 0; i < data_.numNodes; ++i) {
            auto* rowFrame = new QFrame(scrollWidget);
            rowFrame->setStyleSheet(QString(
                "QFrame { background: %1; border-radius: 8px; padding: 6px; }")
                    .arg(kBgPanel.name()));

            auto* rowLay = new QHBoxLayout(rowFrame);
            rowLay->setContentsMargins(12, 8, 12, 8);
            rowLay->setSpacing(12);

            auto* cb = new QCheckBox(QString::fromStdString(data_.nodeNames[i]), rowFrame);
            auto* sevLabel = new QLabel("Severity:", rowFrame);
            sevLabel->setStyleSheet(QString("color: %1; font-size: 12px;").arg(kTextSecondary.name()));
            auto* sevSpin = new QSpinBox(rowFrame);
            sevSpin->setRange(1, 10);
            sevSpin->setValue(5);

            auto* popLabel = new QLabel("Pop:", rowFrame);
            popLabel->setStyleSheet(QString("color: %1; font-size: 12px;").arg(kTextSecondary.name()));
            auto* popSpin = new QSpinBox(rowFrame);
            popSpin->setRange(1, 99999);
            popSpin->setValue(100);

            // Pre-fill if this node is already a disaster site
            if (siteByNode.count(i)) {
                cb->setChecked(true);
                sevSpin->setValue(siteByNode[i]->severity);
                popSpin->setValue(siteByNode[i]->population);
            }

            // Enable/disable spinboxes based on checkbox
            sevSpin->setEnabled(cb->isChecked());
            popSpin->setEnabled(cb->isChecked());
            sevLabel->setEnabled(cb->isChecked());
            popLabel->setEnabled(cb->isChecked());

            connect(cb, &QCheckBox::toggled, this, [=](bool on) {
                sevSpin->setEnabled(on);
                popSpin->setEnabled(on);
                sevLabel->setEnabled(on);
                popLabel->setEnabled(on);
            });

            rowLay->addWidget(cb, 1);
            rowLay->addWidget(sevLabel);
            rowLay->addWidget(sevSpin);
            rowLay->addWidget(popLabel);
            rowLay->addWidget(popSpin);

            grid->addWidget(rowFrame);

            rows[i] = {cb, sevSpin, popSpin};
        }

        auto* scroll = new QScrollArea(dlg);
        scroll->setWidget(scrollWidget);
        scroll->setWidgetResizable(true);
        scroll->setStyleSheet(QString(
            "QScrollArea { border: 1px solid %1; border-radius: 8px; background: %2; }"
            "QScrollBar:vertical { background: %2; width: 8px; border-radius: 4px; }"
            "QScrollBar::handle:vertical { background: %3; border-radius: 4px; min-height: 30px; }")
                .arg(kBorderSubtle.name(), kBgDark.name(), kBorderSubtle.name()));
        lay->addWidget(scroll, 1);

        // Buttons
        auto* btnRow = new QHBoxLayout();
        btnRow->addStretch();

        auto makeDlgBtn = [&](const QString& text, const QColor& accent) {
            auto* b = new QPushButton(text, dlg);
            b->setCursor(Qt::PointingHandCursor);
            b->setStyleSheet(QString(
                "QPushButton { background: %1; color: %2; border: 1px solid %3;"
                " border-radius: 8px; padding: 10px 28px; font-size: 14px; font-weight: 600; }"
                "QPushButton:hover { background: %4; border-color: %5; }")
                    .arg(kBgCard.name(), kTextPrimary.name(), kBorderSubtle.name(),
                         accent.darker(160).name(), accent.name()));
            return b;
        };

        auto* cancelBtn = makeDlgBtn("Cancel", kTextSecondary);
        auto* applyBtn  = makeDlgBtn("✓  Apply", kNodeDisaster);

        connect(cancelBtn, &QPushButton::clicked, dlg, &QDialog::reject);
        connect(applyBtn,  &QPushButton::clicked, dlg, &QDialog::accept);

        btnRow->addWidget(cancelBtn);
        btnRow->addWidget(applyBtn);
        lay->addLayout(btnRow);

        if (dlg->exec() != QDialog::Accepted) return;

        // Apply changes
        data_.sites.clear();
        for (int i = 0; i < data_.numNodes; ++i) {
            if (rows[i].check->isChecked()) {
                DisasterSite site;
                site.nodeId     = i;
                site.name       = data_.nodeNames[i];
                site.severity   = rows[i].severity->value();
                site.population = rows[i].population->value();
                data_.sites.push_back(site);
            }
        }

        // Sort sites using Greedy heuristics instead of naive severity
        auto triageResults = GreedySolver::computeTriage(data_.sites, *graph_, data_.responders);
        data_.sites.clear();
        for (const auto& tr : triageResults) {
            data_.sites.push_back(tr.site);
        }

        // Reclassify and redraw
        roles_ = classifyNodes(data_);
        drawGraph();
        resetHighlights();

        logSiteSummary();
    }

    void onGreedy() {
        if (!ensureLoaded() || data_.sites.empty()) return;

        auto triageResults = GreedySolver::computeTriage(data_.sites, *graph_, data_.responders);

        auto* dlg = new QDialog(this);
        dlg->setWindowTitle("Greedy Triage Priority");
        dlg->resize(680, 400);
        dlg->setStyleSheet(QString(
            "QDialog { background: %1; }"
            "QLabel  { color: %2; }"
            "QTableWidget { background: %3; color: %4;"
            "  border: 1px solid %5; border-radius: 8px;"
            "  font-size: 14px; gridline-color: %5; }"
            "QTableWidget::item { padding: 6px; }"
            "QHeaderView::section { background: %6; color: %7;"
            "  font-weight: 700; font-size: 13px; padding: 8px;"
            "  border: none; border-bottom: 2px solid %8; }")
                .arg(kBgDark.name(), kTextPrimary.name(),
                     kBgPanel.name(), kTextPrimary.name(),
                     kBorderSubtle.name(), kBgCard.name(),
                     kTextPrimary.name(), kNodeDisaster.name()));

        auto* lay = new QVBoxLayout(dlg);
        lay->setContentsMargins(20, 20, 20, 20);
        lay->setSpacing(12);

        auto* header = new QLabel("🏆 Greedy Triage Evaluation", dlg);
        header->setStyleSheet(QString("font-size: 18px; font-weight: 700; color: %1;").arg(kTextPrimary.name()));
        lay->addWidget(header);

        auto* capLbl = new QLabel("Score = (Severity × Population) / (Time to Responder + 1)", dlg);
        capLbl->setStyleSheet(QString("color: %1; font-size: 13px;").arg(kTextSecondary.name()));
        lay->addWidget(capLbl);

        // Table
        int rows = static_cast<int>(triageResults.size());
        auto* table = new QTableWidget(rows, 5, dlg);
        table->setHorizontalHeaderLabels({"Rank", "Site", "Severity", "Nearest Rsp (m)", "Score"});
        table->horizontalHeader()->setStretchLastSection(true);
        table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        table->verticalHeader()->setVisible(false);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table->setSelectionMode(QAbstractItemView::NoSelection);
        table->setShowGrid(false);
        table->setAlternatingRowColors(true);
        table->setStyleSheet(table->styleSheet() + QString("QTableWidget::item:alternate { background: %1; }").arg(kBgCard.name()));

        for (int row = 0; row < rows; ++row) {
            const auto& tr = triageResults[row];
            table->setItem(row, 0, new QTableWidgetItem(QString("#%1").arg(row + 1)));
            table->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(tr.site.name)));
            table->setItem(row, 2, new QTableWidgetItem(QString::number(tr.site.severity) + "/10"));
            table->setItem(row, 3, new QTableWidgetItem(QString::number(tr.nearestResponderTime)));
            
            QTableWidgetItem* scoreItem = new QTableWidgetItem(QString::number(tr.triageScore, 'f', 2));
            scoreItem->setForeground(QBrush(kNodeDisaster));
            table->setItem(row, 4, scoreItem);
        }
        lay->addWidget(table);

        auto* closeBtn = new QPushButton("Close", dlg);
        closeBtn->setStyleSheet(QString(
            "QPushButton { background: %1; color: %2; border: 1px solid %3; border-radius: 8px; padding: 10px 24px; font-size: 14px; font-weight: 600; }"
            "QPushButton:hover { background: %4; }")
                .arg(kBgCard.name(), kTextPrimary.name(), kBorderSubtle.name(), kNodeDisaster.darker(160).name()));
        closeBtn->setCursor(Qt::PointingHandCursor);
        connect(closeBtn, &QPushButton::clicked, dlg, &QDialog::accept);
        lay->addWidget(closeBtn, 0, Qt::AlignRight);

        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->exec();
    }

    void onRunAll() {
        if (isAnimating_) return;
        onLoad();
        
        QTimer::singleShot(500, this, [this]() {
            animateBfs([this]() {
                QTimer::singleShot(800, this, [this]() {
                    animateDijkstra([this]() {
                        QTimer::singleShot(800, this, [this]() {
                            onKnapsack();
                            log("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
                            log("⚡ <b>End-to-end demo complete.</b>");
                        });
                    });
                });
            });
        });
    }

    void onReport() {
        if (!ensureLoaded()) return;

        auto* dlg = new QDialog(this);
        dlg->setWindowTitle("System Status Report");
        dlg->resize(600, 500);
        dlg->setStyleSheet(QString(
            "QDialog { background: %1; }"
            "QLabel  { color: %2; font-weight: bold; font-size: 16px; }"
            "QTextEdit { background: %3; color: %4; border: 1px solid %5;"
            "  border-radius: 8px; font-family: 'JetBrains Mono', monospace; font-size: 13px; padding: 10px; }"
            "QPushButton { background: %6; color: %7; border: 1px solid %8;"
            "  border-radius: 6px; padding: 8px 24px; font-weight: 600; }"
            "QPushButton:hover { background: %9; }")
                .arg(kBgDark.name(), kTextPrimary.name(),
                     kBgPanel.name(), kTextSecondary.name(),
                     kBorderSubtle.name(), kBgCard.name(),
                     kTextPrimary.name(), kBorderSubtle.name(),
                     kAccentPurple.darker(160).name()));

        auto* lay = new QVBoxLayout(dlg);
        lay->addWidget(new QLabel("📋 Disaster Response System Report", dlg));

        auto* text = new QTextEdit(dlg);
        text->setReadOnly(true);

        std::ostringstream ss;
        ss << "========================================\n"
           << "   SMART DISASTER RESPONSE REPORT\n"
           << "========================================\n\n";

        ss << "[ DISASTER SITES ]\n";
        for (const auto& s : data_.sites) {
            ss << " - " << s.name << " (Severity: " << s.severity << "/10, Pop: " << s.population << ")\n";
        }
        ss << "\n[ RESPONDERS ]\n";
        for (const auto& r : data_.responders) {
            ss << " - " << r.name << " (" << r.type << ") at " << data_.nodeNames[r.nodeId] << "\n";
        }
        
        if (!lastAllocation_.empty()) {
            ss << "\n[ RECENT KNAPSACK ALLOCATION ]\nCapacity: " << data_.knapsackCapacity << "\n";
            int totalW = 0, totalV = 0;
            for (int id : lastAllocation_) {
                auto& res = data_.resources[id];
                ss << " - " << res.name << " (wt=" << res.weight << ", val=" << res.value << ")\n";
                totalW += res.weight; totalV += res.value;
            }
            ss << "────────────────────────────────────────\n"
               << "Total Weight: " << totalW << "/" << data_.knapsackCapacity << "  |  Total Value: " << totalV << "\n";
        }

        text->setText(QString::fromStdString(ss.str()));
        lay->addWidget(text);

        auto* btnRow = new QHBoxLayout();
        btnRow->addStretch();
        auto* btnClose = new QPushButton("Close", dlg);
        connect(btnClose, &QPushButton::clicked, dlg, &QDialog::accept);
        btnRow->addWidget(btnClose);
        lay->addLayout(btnRow);

        dlg->exec();
    }

    void onReset() {
        if (!loaded_) return;
        resetHighlights();
        results_->clear();
        log("↺ View reset.");
    }

private:
    // ── Drawing ────────────────────────────────────────────
    void drawGraph() {
        scene_->clear();
        glowNodes_.clear();
        edgeItems_.clear();

        // Draw edges.
        for (int u = 0; u < data_.numNodes; ++u) {
            for (const auto& [v, w] : graph_->getAdj(u)) {
                if (v <= u) continue;

                QPointF p1 = positions_[u], p2 = positions_[v];
                auto* line = scene_->addLine(
                    p1.x(), p1.y(), p2.x(), p2.y(),
                    QPen(kEdgeDefault, 2.0, Qt::SolidLine, Qt::RoundCap));
                edgeItems_[edgeKey(u, v)] = line;

                // Weight badge at midpoint.
                double mx = (p1.x() + p2.x()) / 2.0;
                double my = (p1.y() + p2.y()) / 2.0;

                auto* badge = scene_->addRect(
                    mx - 14, my - 10, 28, 20, QPen(Qt::NoPen),
                    QBrush(kWeightBadge));
                badge->setOpacity(0.9);

                auto* wt = scene_->addText(
                    QString::number(w),
                    QFont("sans-serif", 9, QFont::Bold));
                wt->setDefaultTextColor(kTextSecondary);
                double tw = wt->boundingRect().width();
                wt->setPos(mx - tw / 2.0, my - 10);
            }
        }

        // Draw nodes.
        for (int i = 0; i < data_.numNodes; ++i) {
            QColor c = roleColor(roles_[i]);
            auto* node = new GlowNode(
                positions_[i].x(), positions_[i].y(), NODE_RADIUS, c);
            scene_->addItem(node);
            glowNodes_.push_back(node);

            // ID inside node
            auto* idText = scene_->addText(
                QString::number(i),
                QFont("sans-serif", 11, QFont::Bold));
            idText->setDefaultTextColor(Qt::white);
            double iw = idText->boundingRect().width();
            double ih = idText->boundingRect().height();
            idText->setPos(positions_[i].x() - iw / 2.0,
                           positions_[i].y() - ih / 2.0);

            // Name label below
            auto* label = scene_->addText(
                QString::fromStdString(data_.nodeNames[i]),
                QFont("sans-serif", 9));
            label->setDefaultTextColor(kTextSecondary);
            double lw = label->boundingRect().width();
            label->setPos(positions_[i].x() - lw / 2.0,
                          positions_[i].y() + NODE_RADIUS + 6);

            // Generate Tooltip
            QString tooltip = QString("<b>%1</b>").arg(QString::fromStdString(data_.nodeNames[i]));
            if (roles_[i] == NodeRole::Disaster) {
                for (const auto& s : data_.sites) {
                    if (s.nodeId == i) {
                        tooltip += QString("<br><font color='%1'>🔴 Disaster Site</font>"
                                           "<br>Severity: %2/10"
                                           "<br>Population: %3")
                                   .arg(kNodeDisaster.name())
                                   .arg(s.severity).arg(s.population);
                        break;
                    }
                }
            } else if (roles_[i] == NodeRole::Responder) {
                for (const auto& r : data_.responders) {
                    if (r.nodeId == i) {
                        tooltip += QString("<br><font color='%1'>🟢 Responder Base</font>"
                                           "<br>Unit: %2"
                                           "<br>Type: %3")
                                   .arg(kNodeResponder.name())
                                   .arg(QString::fromStdString(r.name))
                                   .arg(QString::fromStdString(r.type));
                        break;
                    }
                }
            } else {
                tooltip += "<br><font color='gray'>⚪ Neutral Location</font>";
            }
            node->setToolTip(tooltip);
        }
    }

    void highlightNode(int id, const QColor& color) {
        if (id >= 0 && id < static_cast<int>(glowNodes_.size())) {
            glowNodes_[id]->setHighlight(color);
        }
    }

    void highlightEdge(int u, int v, const QColor& color, double width) {
        auto key = edgeKey(u, v);
        if (edgeItems_.count(key)) {
            edgeItems_[key]->setPen(QPen(color, width, Qt::SolidLine, Qt::RoundCap));
        }
    }

    void resetHighlights() {
        for (int i = 0; i < static_cast<int>(glowNodes_.size()); ++i) {
            glowNodes_[i]->resetStyle();
        }
        for (auto& [key, item] : edgeItems_) {
            item->setPen(QPen(kEdgeDefault, 2.0, Qt::SolidLine, Qt::RoundCap));
        }
    }

    static uint64_t edgeKey(int u, int v) {
        if (u > v) std::swap(u, v);
        return (static_cast<uint64_t>(u) << 32) | static_cast<uint64_t>(v);
    }

    void log(const std::string& msg) {
        results_->append(QString::fromStdString(msg));
        // Auto-scroll
        auto* sb = results_->verticalScrollBar();
        sb->setValue(sb->maximum());
    }

    bool ensureLoaded() {
        if (!loaded_) { log("⚠️  Click <b>Load Scenario</b> first."); return false; }
        return true;
    }

    // ── Helpers ─────────────────────────────────────────────
    int getHighestSeverityOrigin() const {
        if (data_.sites.empty()) return 0;
        // Sites are sorted by severity descending, so first is highest
        return data_.sites[0].nodeId;
    }

    void logSiteSummary() {
        log("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
        log("📦 <b>Scenario:</b> " + std::to_string(data_.numNodes)
            + " locations, " + std::to_string(data_.edges.size()) + " roads");
        if (data_.sites.empty()) {
            log("⚠️  <b>No disaster sites configured.</b> Click Configure.");
        } else {
            log("🔴 <b>Disaster sites</b> (sorted by severity):");
            for (size_t i = 0; i < data_.sites.size(); ++i) {
                const auto& s = data_.sites[i];
                std::ostringstream oss;
                oss << "   " << (i + 1) << ". " << s.name
                    << "  —  severity <b>" << s.severity
                    << "/10</b>, " << s.population << " affected";
                if (i == 0) oss << "  ← <b>primary target</b>";
                log(oss.str());
            }
        }
        log("🟢 <b>Responders:</b>");
        for (const auto& r : data_.responders) {
            std::ostringstream oss;
            oss << "   • " << r.name << " (" << r.type << ") at "
                << data_.nodeNames[r.nodeId];
            log(oss.str());
        }
    }

    // ── State ──────────────────────────────────────────────
    InputData              data_{};
    std::unique_ptr<Graph> graph_;
    std::vector<QPointF>   positions_;
    std::vector<NodeRole>  roles_;
    bool                   loaded_ = false;

    QGraphicsScene* scene_   = nullptr;
    QGraphicsView*  view_    = nullptr;
    QTextEdit*      results_ = nullptr;

    QPushButton* btnLoad_        = nullptr;
    QPushButton* btnConfigSites_ = nullptr;
    QPushButton* btnGreedy_      = nullptr;
    QPushButton* btnBfs_         = nullptr;
    QPushButton* btnDijkstra_    = nullptr;
    QPushButton* btnKnapsack_    = nullptr;
    QPushButton* btnViewAlloc_   = nullptr;
    QPushButton* btnRunAll_      = nullptr;
    QPushButton* btnReset_       = nullptr;
    QPushButton* btnReport_      = nullptr;

    std::vector<int> lastAllocation_;
    bool isAnimating_ = false;

    void setControlsEnabled(bool enabled) {
        btnLoad_->setEnabled(enabled);
        btnConfigSites_->setEnabled(enabled && loaded_);
        btnGreedy_->setEnabled(enabled);
        btnBfs_->setEnabled(enabled);
        btnDijkstra_->setEnabled(enabled);
        btnKnapsack_->setEnabled(enabled);
        btnViewAlloc_->setEnabled(enabled && !lastAllocation_.empty());
        btnRunAll_->setEnabled(enabled);
        btnReset_->setEnabled(enabled);
        btnReport_->setEnabled(enabled && loaded_);
    }

    std::vector<GlowNode*>                            glowNodes_;
    std::unordered_map<uint64_t, QGraphicsLineItem*>  edgeItems_;
};

// ── Qt MOC include ─────────────────────────────────────────
#include "ui_main.moc"

// ── Entry point ────────────────────────────────────────────
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setStyle("Fusion");

    // Global dark palette
    QPalette pal;
    pal.setColor(QPalette::Window,          kBgDark);
    pal.setColor(QPalette::WindowText,      kTextPrimary);
    pal.setColor(QPalette::Base,            kBgPanel);
    pal.setColor(QPalette::AlternateBase,   kBgDark);
    pal.setColor(QPalette::Text,            kTextPrimary);
    pal.setColor(QPalette::Button,          kBgCard);
    pal.setColor(QPalette::ButtonText,      kTextPrimary);
    pal.setColor(QPalette::Highlight,       kAccentPurple);
    pal.setColor(QPalette::HighlightedText, Qt::white);
    pal.setColor(QPalette::ToolTipBase,     kBgCard);
    pal.setColor(QPalette::ToolTipText,     kTextPrimary);
    app.setPalette(pal);

    DisasterUI window;
    window.show();
    return app.exec();
}
