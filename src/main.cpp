/*
 * ParolTest v2.0 — GTK3 GUI
 * Kompilyatsiya:
 *   g++ -std=c++17 -O2 $(pkg-config --cflags --libs gtk+-3.0) -o paroltest main.cpp
 *
 * O'rnatish (Ubuntu/Debian):
 *   sudo apt install libgtk-3-dev
 *
 * O'rnatish (Fedora/RHEL):
 *   sudo dnf install gtk3-devel
 */

#include <gtk/gtk.h>
#include <string>
#include <vector>
#include <sstream>
#include <cstring>
#include "analyzer.hpp"

// ════════════════════════════════════════════════════════════════
//  Global widget ko'rsatgichlari
// ════════════════════════════════════════════════════════════════

struct AppWidgets {
    GtkWidget* window;
    GtkWidget* passwordEntry;
    GtkWidget* toggleVisBtn;

    // Strength bar
    GtkWidget* strengthBar;
    GtkWidget* strengthLabel;
    GtkWidget* starsLabel;
    GtkWidget* scoreLabel;

    // Metrika kartochkalari
    GtkWidget* lengthVal;
    GtkWidget* charsetVal;
    GtkWidget* entropyVal;
    GtkWidget* entropyBar;

    // Crack time
    GtkWidget* crackTimeVal;
    GtkWidget* crackTypeVal;
    GtkWidget* humorLabel;

    // Muammolar va tavsiyalar
    GtkWidget* commonBadge;
    GtkWidget* issuesBox;
    GtkWidget* tipsBox;

    // Tavsiya parollar
    GtkWidget* suggestionsBox;
    GtkWidget* generatedBox;

    // Status bar
    GtkWidget* statusBar;

    bool passwordVisible = false;
};

static AppWidgets app;

// ════════════════════════════════════════════════════════════════
//  CSS uslublari
// ════════════════════════════════════════════════════════════════

static const char* CSS = R"css(
* {
    font-family: 'Ubuntu Mono', 'DejaVu Sans Mono', monospace;
}

window {
    background-color: #0f1117;
}

.sidebar {
    background-color: #161b27;
    border-right: 1px solid #2a2e3d;
    min-width: 260px;
}

.main-panel {
    background-color: #0f1117;
}

.header-box {
    background-color: #161b27;
    padding: 18px 20px 14px 20px;
    border-bottom: 1px solid #2a2e3d;
}

.app-title {
    font-size: 20px;
    font-weight: bold;
    color: #64d4f7;
    letter-spacing: 2px;
}

.app-subtitle {
    font-size: 11px;
    color: #4a5568;
    margin-top: 2px;
}

.section-title {
    font-size: 11px;
    font-weight: bold;
    color: #4a5568;
    letter-spacing: 1.5px;
    padding: 12px 16px 6px 16px;
    text-transform: uppercase;
}

/* Parol kiritish maydoni */
.password-frame {
    background-color: #161b27;
    border: 1px solid #2a2e3d;
    border-radius: 8px;
    margin: 12px 16px 6px 16px;
}

.password-entry {
    background-color: transparent;
    color: #e2e8f0;
    font-size: 15px;
    font-family: 'Ubuntu Mono', monospace;
    border: none;
    padding: 10px 12px;
    caret-color: #64d4f7;
}

.password-entry:focus {
    background-color: transparent;
    box-shadow: none;
    outline: none;
}

.toggle-btn {
    background-color: transparent;
    border: none;
    color: #4a5568;
    padding: 4px 8px;
    min-width: 0;
    min-height: 0;
}

.toggle-btn:hover {
    color: #64d4f7;
    background-color: transparent;
}

/* Strength bar */
.strength-section {
    padding: 8px 16px 4px 16px;
}

.strength-label-very-weak  { color: #f87171; font-weight: bold; font-size: 13px; }
.strength-label-weak       { color: #fb923c; font-weight: bold; font-size: 13px; }
.strength-label-medium     { color: #fbbf24; font-weight: bold; font-size: 13px; }
.strength-label-strong     { color: #4ade80; font-weight: bold; font-size: 13px; }
.strength-label-very-strong{ color: #34d399; font-weight: bold; font-size: 13px; }

progress, trough {
    min-height: 6px;
    border-radius: 3px;
}

progress.very-weak   { background-color: #f87171; }
progress.weak        { background-color: #fb923c; }
progress.medium      { background-color: #fbbf24; }
progress.strong      { background-color: #4ade80; }
progress.very-strong { background-color: #34d399; }

trough { background-color: #1e2435; border-radius: 3px; }

.stars-label {
    font-size: 15px;
    color: #fbbf24;
    padding: 2px 0;
}

.score-label {
    font-size: 11px;
    color: #4a5568;
}

/* Metrika kartochkalari */
.metric-card {
    background-color: #161b27;
    border: 1px solid #2a2e3d;
    border-radius: 8px;
    padding: 10px 12px;
    margin: 4px;
}

.metric-key {
    font-size: 10px;
    color: #4a5568;
    letter-spacing: 1px;
    text-transform: uppercase;
}

.metric-val {
    font-size: 14px;
    color: #e2e8f0;
    font-weight: bold;
    margin-top: 2px;
}

.metric-val-good   { color: #34d399; font-size: 14px; font-weight: bold; }
.metric-val-warn   { color: #fbbf24; font-size: 14px; font-weight: bold; }
.metric-val-danger { color: #f87171; font-size: 14px; font-weight: bold; }

/* Crack time */
.crack-section {
    background-color: #161b27;
    border: 1px solid #2a2e3d;
    border-radius: 8px;
    margin: 4px 16px;
    padding: 12px 14px;
}

.crack-time-val {
    font-size: 18px;
    font-weight: bold;
    color: #64d4f7;
}

.crack-attack-type {
    font-size: 10px;
    color: #4a5568;
    letter-spacing: 1px;
}

.humor-label {
    font-size: 12px;
    color: #718096;
    padding: 6px 0 2px 0;
    font-style: italic;
}

/* Badge */
.badge-common {
    background-color: #2d1b1b;
    color: #f87171;
    border: 1px solid #5c2626;
    border-radius: 4px;
    padding: 3px 8px;
    font-size: 11px;
    font-weight: bold;
}

.badge-safe {
    background-color: #1a2d1e;
    color: #4ade80;
    border: 1px solid #1f4a25;
    border-radius: 4px;
    padding: 3px 8px;
    font-size: 11px;
    font-weight: bold;
}

/* Issue va tips */
.issue-label {
    font-size: 12px;
    color: #fb923c;
    padding: 3px 0;
}

.issue-label:before { content: "⚠ "; }

.tip-label {
    font-size: 12px;
    color: #94a3b8;
    padding: 3px 0;
}

/* Suggestion parollar */
.suggest-row {
    background-color: #161b27;
    border: 1px solid #2a2e3d;
    border-radius: 6px;
    margin: 3px 0;
    padding: 8px 10px;
}

.suggest-pass {
    font-family: 'Ubuntu Mono', monospace;
    font-size: 13px;
    color: #e2e8f0;
    letter-spacing: 0.5px;
}

.suggest-tag {
    font-size: 10px;
    color: #4a5568;
}

.copy-btn {
    background-color: #1e2435;
    border: 1px solid #2a2e3d;
    border-radius: 4px;
    color: #94a3b8;
    padding: 3px 8px;
    font-size: 11px;
    min-height: 0;
    min-width: 0;
}

.copy-btn:hover {
    background-color: #2a3450;
    color: #64d4f7;
    border-color: #64d4f7;
}

/* Ajratuvchi chiziq */
.divider {
    background-color: #2a2e3d;
    min-height: 1px;
    margin: 8px 16px;
}

/* Generator tugmasi */
.gen-btn {
    background-color: #0d2137;
    border: 1px solid #1e4d6b;
    border-radius: 6px;
    color: #64d4f7;
    font-size: 12px;
    padding: 7px 14px;
    margin: 4px 16px;
}

.gen-btn:hover {
    background-color: #1a3a52;
    border-color: #64d4f7;
}

/* Scroll */
scrolledwindow {
    background-color: transparent;
}

viewport {
    background-color: transparent;
}

/* Status bar */
.statusbar {
    background-color: #0a0d14;
    border-top: 1px solid #2a2e3d;
    font-size: 11px;
    color: #4a5568;
    padding: 4px 12px;
}
)css";

// ════════════════════════════════════════════════════════════════
//  Yordamchi funksiyalar
// ════════════════════════════════════════════════════════════════

static void applyClass(GtkWidget* w, const char* cls) {
    gtk_style_context_add_class(gtk_widget_get_style_context(w), cls);
}

static void removeAllClasses(GtkWidget* w, const char* prefix) {
    GtkStyleContext* ctx = gtk_widget_get_style_context(w);
    static const char* cls[] = {"very-weak","weak","medium","strong","very-strong","empty",nullptr};
    for (int i = 0; cls[i]; i++) gtk_style_context_remove_class(ctx, cls[i]);
}

static void clearBox(GtkWidget* box) {
    GList* children = gtk_container_get_children(GTK_CONTAINER(box));
    for (GList* c = children; c; c = c->next)
        gtk_widget_destroy(GTK_WIDGET(c->data));
    g_list_free(children);
}

static void clipboardCopy(const std::string& text) {
    GtkClipboard* clip = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    gtk_clipboard_set_text(clip, text.c_str(), -1);
}

// ════════════════════════════════════════════════════════════════
//  Copy tugmasi callback
// ════════════════════════════════════════════════════════════════

static void onCopyClicked(GtkButton* btn, gpointer data) {
    const char* pass = (const char*)data;
    clipboardCopy(pass);
    gtk_button_set_label(btn, "✓ Nusxalandi");
    // 1.5 s dan so'ng qaytarish
    g_timeout_add(1500, [](gpointer d) -> gboolean {
        gtk_button_set_label(GTK_BUTTON(d), "Nusxa");
        return G_SOURCE_REMOVE;
    }, btn);
}

// ════════════════════════════════════════════════════════════════
//  Tavsiya parol qatori
// ════════════════════════════════════════════════════════════════

static GtkWidget* makeSuggestRow(const std::string& pass, const std::string& tag) {
    GtkWidget* row  = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    applyClass(row, "suggest-row");

    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_hexpand(vbox, TRUE);

    GtkWidget* passLbl = gtk_label_new(pass.c_str());
    gtk_label_set_xalign(GTK_LABEL(passLbl), 0.0f);
    gtk_label_set_selectable(GTK_LABEL(passLbl), TRUE);
    applyClass(passLbl, "suggest-pass");

    GtkWidget* tagLbl = gtk_label_new(tag.c_str());
    gtk_label_set_xalign(GTK_LABEL(tagLbl), 0.0f);
    applyClass(tagLbl, "suggest-tag");

    gtk_box_pack_start(GTK_BOX(vbox), passLbl, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), tagLbl,  FALSE, FALSE, 0);

    // Copy tugmasi (parolni statik saqlaymiz)
    char* passCopy = g_strdup(pass.c_str());
    GtkWidget* copyBtn = gtk_button_new_with_label("Nusxa");
    applyClass(copyBtn, "copy-btn");
    gtk_widget_set_valign(copyBtn, GTK_ALIGN_CENTER);
    g_signal_connect(copyBtn, "clicked", G_CALLBACK(onCopyClicked), passCopy);
    g_object_set_data_full(G_OBJECT(row), "pass-copy", passCopy, g_free);

    gtk_box_pack_start(GTK_BOX(row), vbox,    TRUE,  TRUE,  0);
    gtk_box_pack_start(GTK_BOX(row), copyBtn, FALSE, FALSE, 0);

    return row;
}

// ════════════════════════════════════════════════════════════════
//  UI yangilash — asosiy funksiya
// ════════════════════════════════════════════════════════════════

static void updateUI(const StrengthResult& r) {
    // ── Strength bar ──────────────────────────────────────────
    removeAllClasses(app.strengthBar, nullptr);
    applyClass(app.strengthBar, r.cssClass.c_str());
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(app.strengthBar),
                                  r.score / 100.0);

    removeAllClasses(app.strengthLabel, nullptr);
    std::string slClass = "strength-label-" + r.cssClass;
    applyClass(app.strengthLabel, slClass.c_str());
    gtk_label_set_text(GTK_LABEL(app.strengthLabel), r.label.c_str());

    // Yulduzlar
    std::string stars;
    for (int i = 0; i < 5; i++) stars += (i < r.stars) ? "★" : "☆";
    gtk_label_set_text(GTK_LABEL(app.starsLabel), stars.c_str());

    std::string scoreText = std::to_string(r.score) + " / 100";
    gtk_label_set_text(GTK_LABEL(app.scoreLabel), scoreText.c_str());

    // ── Uzunlik ───────────────────────────────────────────────
    {
        std::string v = std::to_string(r.crack.seconds < 1
                            ? (int)r.entropy // placeholder
                            : (int)r.entropy) + " belgi... "; // stub
        // Aslida parolni saqlashdan olamiz
    }

    // ── Charset ──────────────────────────────────────────────
    std::string cs;
    if (r.charset.hasLower)  cs += "a-z ";
    if (r.charset.hasUpper)  cs += "A-Z ";
    if (r.charset.hasDigit)  cs += "0-9 ";
    if (r.charset.hasSymbol) cs += "!@# ";
    if (cs.empty()) cs = "—";
    gtk_label_set_text(GTK_LABEL(app.charsetVal), cs.c_str());

    // ── Entropy ──────────────────────────────────────────────
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(1) << r.entropy << " bit";
        gtk_label_set_text(GTK_LABEL(app.entropyVal), oss.str().c_str());

        // Entropy rang
        GtkStyleContext* ctx = gtk_widget_get_style_context(app.entropyVal);
        gtk_style_context_remove_class(ctx, "metric-val-good");
        gtk_style_context_remove_class(ctx, "metric-val-warn");
        gtk_style_context_remove_class(ctx, "metric-val-danger");
        if (r.entropy >= 70)      gtk_style_context_add_class(ctx, "metric-val-good");
        else if (r.entropy >= 40) gtk_style_context_add_class(ctx, "metric-val-warn");
        else                      gtk_style_context_add_class(ctx, "metric-val-danger");

        // Entropy progress bar
        double frac = std::min(1.0, r.entropy / 100.0);
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(app.entropyBar), frac);
    }

    // ── Crack time ───────────────────────────────────────────
    gtk_label_set_text(GTK_LABEL(app.crackTimeVal), r.crack.label.c_str());
    gtk_label_set_text(GTK_LABEL(app.crackTypeVal), r.crack.attackType.c_str());
    gtk_label_set_text(GTK_LABEL(app.humorLabel),   r.crack.humor.c_str());

    // Crack time rangi
    {
        GtkStyleContext* ctx = gtk_widget_get_style_context(app.crackTimeVal);
        gtk_style_context_remove_class(ctx, "metric-val-good");
        gtk_style_context_remove_class(ctx, "metric-val-warn");
        gtk_style_context_remove_class(ctx, "metric-val-danger");
        gtk_style_context_remove_class(ctx, "crack-time-val");
        if (r.crack.severity >= 4)      gtk_style_context_add_class(ctx, "metric-val-good");
        else if (r.crack.severity >= 2) gtk_style_context_add_class(ctx, "metric-val-warn");
        else                            gtk_style_context_add_class(ctx, "metric-val-danger");
    }

    // ── Baza badge ───────────────────────────────────────────
    GtkStyleContext* badgeCtx = gtk_widget_get_style_context(app.commonBadge);
    gtk_style_context_remove_class(badgeCtx, "badge-common");
    gtk_style_context_remove_class(badgeCtx, "badge-safe");
    if (r.isCommon) {
        gtk_label_set_text(GTK_LABEL(app.commonBadge), "⚠  Bazada topildi!");
        gtk_style_context_add_class(badgeCtx, "badge-common");
    } else {
        gtk_label_set_text(GTK_LABEL(app.commonBadge), "✓  Bazada yo'q");
        gtk_style_context_add_class(badgeCtx, "badge-safe");
    }

    // ── Muammolar ─────────────────────────────────────────────
    clearBox(app.issuesBox);
    if (r.issues.empty()) {
        GtkWidget* lbl = gtk_label_new("Muammo aniqlanmadi");
        gtk_label_set_xalign(GTK_LABEL(lbl), 0.0f);
        applyClass(lbl, "tip-label");
        gtk_box_pack_start(GTK_BOX(app.issuesBox), lbl, FALSE, FALSE, 0);
    } else {
        for (auto& iss : r.issues) {
            GtkWidget* lbl = gtk_label_new(("⚠  " + iss).c_str());
            gtk_label_set_xalign(GTK_LABEL(lbl), 0.0f);
            gtk_label_set_line_wrap(GTK_LABEL(lbl), TRUE);
            applyClass(lbl, "issue-label");
            gtk_box_pack_start(GTK_BOX(app.issuesBox), lbl, FALSE, FALSE, 0);
        }
    }

    // ── Tavsiyalar ────────────────────────────────────────────
    clearBox(app.tipsBox);
    if (r.tips.empty()) {
        GtkWidget* lbl = gtk_label_new("✓  Parol barcha talablarga javob beradi");
        gtk_label_set_xalign(GTK_LABEL(lbl), 0.0f);
        applyClass(lbl, "tip-label");
        gtk_box_pack_start(GTK_BOX(app.tipsBox), lbl, FALSE, FALSE, 0);
    } else {
        for (auto& tip : r.tips) {
            GtkWidget* lbl = gtk_label_new(("→  " + tip).c_str());
            gtk_label_set_xalign(GTK_LABEL(lbl), 0.0f);
            gtk_label_set_line_wrap(GTK_LABEL(lbl), TRUE);
            applyClass(lbl, "tip-label");
            gtk_box_pack_start(GTK_BOX(app.tipsBox), lbl, FALSE, FALSE, 0);
        }
    }

    // ── Kuchaytirish variantlari ──────────────────────────────
    clearBox(app.suggestionsBox);
    for (size_t i = 0; i < r.suggestions.size(); i++) {
        std::string tag = (i < 2) ? "Asosiy paroldan" : "Passphrase uslubi";
        GtkWidget* row = makeSuggestRow(r.suggestions[i], tag);
        gtk_box_pack_start(GTK_BOX(app.suggestionsBox), row, FALSE, FALSE, 0);
    }

    // ── Tasodifiy kuchli parollar ─────────────────────────────
    clearBox(app.generatedBox);
    static const char* genTags[] = {"14 belgi", "16 belgi", "18 belgi"};
    for (size_t i = 0; i < r.generated.size(); i++) {
        std::string tag = std::string(genTags[i]) + " — Kriptografik";
        GtkWidget* row = makeSuggestRow(r.generated[i], tag);
        gtk_box_pack_start(GTK_BOX(app.generatedBox), row, FALSE, FALSE, 0);
    }

    gtk_widget_show_all(app.window);
}

// ════════════════════════════════════════════════════════════════
//  Parol o'zgarganda
// ════════════════════════════════════════════════════════════════

static void onPasswordChanged(GtkEntry* entry, gpointer) {
    const char* text = gtk_entry_get_text(entry);
    std::string password(text ? text : "");

    // Uzunlikni strength bar yonida ko'rsat
    std::string lenInfo = std::to_string(password.size()) + " belgi";
    gtk_label_set_text(GTK_LABEL(app.lengthVal), lenInfo.c_str());

    if (password.empty()) {
        // Reset
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(app.strengthBar), 0.0);
        gtk_label_set_text(GTK_LABEL(app.strengthLabel), "—");
        gtk_label_set_text(GTK_LABEL(app.starsLabel), "☆☆☆☆☆");
        gtk_label_set_text(GTK_LABEL(app.scoreLabel), "0 / 100");
        return;
    }

    StrengthResult r = analyze(password);
    updateUI(r);

    // Status bar
    std::ostringstream sb;
    sb << "Parol tahlil qilindi  |  "
       << r.score << "/100 ball  |  "
       << std::fixed << std::setprecision(1) << r.entropy << " bit  |  "
       << r.crack.label;
    gtk_statusbar_push(GTK_STATUSBAR(app.statusBar), 0, sb.str().c_str());
}

// ════════════════════════════════════════════════════════════════
//  Ko'rsat/Yashir tugmasi
// ════════════════════════════════════════════════════════════════

static void onToggleVisible(GtkButton*, gpointer) {
    app.passwordVisible = !app.passwordVisible;
    gtk_entry_set_visibility(GTK_ENTRY(app.passwordEntry), app.passwordVisible);
    gtk_button_set_label(GTK_BUTTON(app.toggleVisBtn),
                         app.passwordVisible ? "🙈" : "👁");
}

// ════════════════════════════════════════════════════════════════
//  Generator tugmasi
// ════════════════════════════════════════════════════════════════

static void onGenerateClicked(GtkButton*, gpointer) {
    std::string pass = generateStrong(16);
    gtk_entry_set_text(GTK_ENTRY(app.passwordEntry), pass.c_str());
}

// ════════════════════════════════════════════════════════════════
//  Kartochka yaratish yordamchisi
// ════════════════════════════════════════════════════════════════

static GtkWidget* makeMetricCard(const char* keyText,
                                  GtkWidget** valOut,
                                  const char* valClass = "metric-val") {
    GtkWidget* card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    applyClass(card, "metric-card");
    gtk_widget_set_hexpand(card, TRUE);

    GtkWidget* key = gtk_label_new(keyText);
    gtk_label_set_xalign(GTK_LABEL(key), 0.0f);
    applyClass(key, "metric-key");

    GtkWidget* val = gtk_label_new("—");
    gtk_label_set_xalign(GTK_LABEL(val), 0.0f);
    applyClass(val, valClass);
    gtk_label_set_selectable(GTK_LABEL(val), TRUE);

    gtk_box_pack_start(GTK_BOX(card), key, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(card), val, FALSE, FALSE, 0);

    if (valOut) *valOut = val;
    return card;
}

// ════════════════════════════════════════════════════════════════
//  Sidebar (chap panel)
// ════════════════════════════════════════════════════════════════

static GtkWidget* buildSidebar() {
    GtkWidget* sidebar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    applyClass(sidebar, "sidebar");

    // ── Logo/Header ───────────────────────────────────────────
    GtkWidget* header = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    applyClass(header, "header-box");

    GtkWidget* title = gtk_label_new("🔐 PAROLTEST");
    gtk_label_set_xalign(GTK_LABEL(title), 0.0f);
    applyClass(title, "app-title");

    GtkWidget* sub = gtk_label_new("Parol kuchini tahlil qilish");
    gtk_label_set_xalign(GTK_LABEL(sub), 0.0f);
    applyClass(sub, "app-subtitle");

    gtk_box_pack_start(GTK_BOX(header), title, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(header), sub,   FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(sidebar), header, FALSE, FALSE, 0);

    // ── Parol kiritish ─────────────────────────────────────────
    GtkWidget* secTitle1 = gtk_label_new("PAROL");
    gtk_label_set_xalign(GTK_LABEL(secTitle1), 0.0f);
    applyClass(secTitle1, "section-title");
    gtk_box_pack_start(GTK_BOX(sidebar), secTitle1, FALSE, FALSE, 0);

    GtkWidget* entryFrame = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    applyClass(entryFrame, "password-frame");

    app.passwordEntry = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(app.passwordEntry), FALSE);
    gtk_entry_set_placeholder_text(GTK_ENTRY(app.passwordEntry), "Parolni kiriting...");
    applyClass(app.passwordEntry, "password-entry");
    gtk_widget_set_hexpand(app.passwordEntry, TRUE);

    app.toggleVisBtn = gtk_button_new_with_label("👁");
    applyClass(app.toggleVisBtn, "toggle-btn");
    g_signal_connect(app.toggleVisBtn, "clicked", G_CALLBACK(onToggleVisible), nullptr);

    gtk_box_pack_start(GTK_BOX(entryFrame), app.passwordEntry, TRUE,  TRUE,  0);
    gtk_box_pack_start(GTK_BOX(entryFrame), app.toggleVisBtn,  FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(sidebar), entryFrame, FALSE, FALSE, 0);

    // ── Generator tugmasi ─────────────────────────────────────
    GtkWidget* genBtn = gtk_button_new_with_label("⚡  Kuchli parol yaratish");
    applyClass(genBtn, "gen-btn");
    g_signal_connect(genBtn, "clicked", G_CALLBACK(onGenerateClicked), nullptr);
    gtk_box_pack_start(GTK_BOX(sidebar), genBtn, FALSE, FALSE, 0);

    // ── Strength ─────────────────────────────────────────────
    GtkWidget* divider1 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    applyClass(divider1, "divider");
    gtk_box_pack_start(GTK_BOX(sidebar), divider1, FALSE, FALSE, 0);

    GtkWidget* secTitle2 = gtk_label_new("KUCH DARAJASI");
    gtk_label_set_xalign(GTK_LABEL(secTitle2), 0.0f);
    applyClass(secTitle2, "section-title");
    gtk_box_pack_start(GTK_BOX(sidebar), secTitle2, FALSE, FALSE, 0);

    GtkWidget* strengthSec = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    applyClass(strengthSec, "strength-section");

    // Label qatori
    GtkWidget* labelRow = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    app.strengthLabel = gtk_label_new("—");
    gtk_label_set_xalign(GTK_LABEL(app.strengthLabel), 0.0f);
    gtk_widget_set_hexpand(app.strengthLabel, TRUE);
    applyClass(app.strengthLabel, "strength-label-medium");

    app.scoreLabel = gtk_label_new("0 / 100");
    applyClass(app.scoreLabel, "score-label");

    gtk_box_pack_start(GTK_BOX(labelRow), app.strengthLabel, TRUE,  TRUE,  0);
    gtk_box_pack_start(GTK_BOX(labelRow), app.scoreLabel,    FALSE, FALSE, 0);

    app.strengthBar = gtk_progress_bar_new();
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(app.strengthBar), 0.0);

    app.starsLabel = gtk_label_new("☆☆☆☆☆");
    gtk_label_set_xalign(GTK_LABEL(app.starsLabel), 0.0f);
    applyClass(app.starsLabel, "stars-label");

    gtk_box_pack_start(GTK_BOX(strengthSec), labelRow,       FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(strengthSec), app.strengthBar,FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(strengthSec), app.starsLabel, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(sidebar), strengthSec, FALSE, FALSE, 0);

    // ── Metrika kartochkalari ──────────────────────────────────
    GtkWidget* divider2 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    applyClass(divider2, "divider");
    gtk_box_pack_start(GTK_BOX(sidebar), divider2, FALSE, FALSE, 0);

    GtkWidget* secTitle3 = gtk_label_new("METRIKALAR");
    gtk_label_set_xalign(GTK_LABEL(secTitle3), 0.0f);
    applyClass(secTitle3, "section-title");
    gtk_box_pack_start(GTK_BOX(sidebar), secTitle3, FALSE, FALSE, 0);

    GtkWidget* metricPad = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_margin_start(metricPad, 16);
    gtk_widget_set_margin_end(metricPad, 16);

    GtkWidget* row1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_box_pack_start(GTK_BOX(row1), makeMetricCard("UZUNLIK",  &app.lengthVal),  TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(row1), makeMetricCard("CHARSET",  &app.charsetVal), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(metricPad), row1, FALSE, FALSE, 0);

    // Entropy card + mini bar
    GtkWidget* entCard = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    applyClass(entCard, "metric-card");

    GtkWidget* entKey = gtk_label_new("ENTROPY");
    gtk_label_set_xalign(GTK_LABEL(entKey), 0.0f);
    applyClass(entKey, "metric-key");

    GtkWidget* entRow = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    app.entropyVal = gtk_label_new("—");
    gtk_label_set_xalign(GTK_LABEL(app.entropyVal), 0.0f);
    gtk_widget_set_hexpand(app.entropyVal, TRUE);
    applyClass(app.entropyVal, "metric-val-danger");

    app.entropyBar = gtk_progress_bar_new();
    gtk_widget_set_size_request(app.entropyBar, 80, -1);

    gtk_box_pack_start(GTK_BOX(entRow), app.entropyVal,  TRUE,  TRUE,  0);
    gtk_box_pack_start(GTK_BOX(entRow), app.entropyBar,  FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(entCard), entKey, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(entCard), entRow, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(metricPad), entCard, FALSE, FALSE, 4);

    gtk_box_pack_start(GTK_BOX(sidebar), metricPad, FALSE, FALSE, 0);

    // ── Crack time ────────────────────────────────────────────
    GtkWidget* divider3 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    applyClass(divider3, "divider");
    gtk_box_pack_start(GTK_BOX(sidebar), divider3, FALSE, FALSE, 0);

    GtkWidget* secTitle4 = gtk_label_new("BUZILISH VAQTI");
    gtk_label_set_xalign(GTK_LABEL(secTitle4), 0.0f);
    applyClass(secTitle4, "section-title");
    gtk_box_pack_start(GTK_BOX(sidebar), secTitle4, FALSE, FALSE, 0);

    GtkWidget* crackBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    applyClass(crackBox, "crack-section");
    gtk_widget_set_margin_start(crackBox, 16);
    gtk_widget_set_margin_end(crackBox, 16);

    app.crackTimeVal = gtk_label_new("—");
    gtk_label_set_xalign(GTK_LABEL(app.crackTimeVal), 0.0f);
    applyClass(app.crackTimeVal, "crack-time-val");
    gtk_label_set_selectable(GTK_LABEL(app.crackTimeVal), TRUE);

    app.crackTypeVal = gtk_label_new("GPU Brute-force");
    gtk_label_set_xalign(GTK_LABEL(app.crackTypeVal), 0.0f);
    applyClass(app.crackTypeVal, "crack-attack-type");

    app.humorLabel = gtk_label_new("");
    gtk_label_set_xalign(GTK_LABEL(app.humorLabel), 0.0f);
    gtk_label_set_line_wrap(GTK_LABEL(app.humorLabel), TRUE);
    gtk_widget_set_size_request(app.humorLabel, 220, -1);
    applyClass(app.humorLabel, "humor-label");

    gtk_box_pack_start(GTK_BOX(crackBox), app.crackTimeVal, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(crackBox), app.crackTypeVal, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(crackBox), app.humorLabel,   FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(sidebar), crackBox, FALSE, FALSE, 0);

    // ── Baza badge ────────────────────────────────────────────
    GtkWidget* badgePad = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_margin_start(badgePad, 16);
    gtk_widget_set_margin_end(badgePad, 16);
    gtk_widget_set_margin_top(badgePad, 8);

    app.commonBadge = gtk_label_new("—");
    gtk_label_set_xalign(GTK_LABEL(app.commonBadge), 0.0f);
    applyClass(app.commonBadge, "badge-safe");
    gtk_box_pack_start(GTK_BOX(badgePad), app.commonBadge, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(sidebar), badgePad, FALSE, FALSE, 0);

    // Ajratuvchi
    GtkWidget* spacer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_vexpand(spacer, TRUE);
    gtk_box_pack_start(GTK_BOX(sidebar), spacer, TRUE, TRUE, 0);

    g_signal_connect(app.passwordEntry, "changed",
                     G_CALLBACK(onPasswordChanged), nullptr);

    return sidebar;
}

// ════════════════════════════════════════════════════════════════
//  O'ng panel (tavsiyalar)
// ════════════════════════════════════════════════════════════════

static GtkWidget* buildMainPanel() {
    GtkWidget* scroll = gtk_scrolled_window_new(nullptr, nullptr);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    applyClass(vbox, "main-panel");
    gtk_widget_set_margin_start(vbox, 4);

    // ── Muammolar ─────────────────────────────────────────────
    GtkWidget* t1 = gtk_label_new("MUAMMOLAR");
    gtk_label_set_xalign(GTK_LABEL(t1), 0.0f);
    applyClass(t1, "section-title");
    gtk_box_pack_start(GTK_BOX(vbox), t1, FALSE, FALSE, 0);

    app.issuesBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_widget_set_margin_start(app.issuesBox, 16);
    gtk_widget_set_margin_end(app.issuesBox, 16);
    gtk_widget_set_margin_bottom(app.issuesBox, 4);
    gtk_box_pack_start(GTK_BOX(vbox), app.issuesBox, FALSE, FALSE, 0);

    GtkWidget* sep1 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    applyClass(sep1, "divider");
    gtk_box_pack_start(GTK_BOX(vbox), sep1, FALSE, FALSE, 0);

    // ── Tavsiyalar ────────────────────────────────────────────
    GtkWidget* t2 = gtk_label_new("TAVSIYALAR");
    gtk_label_set_xalign(GTK_LABEL(t2), 0.0f);
    applyClass(t2, "section-title");
    gtk_box_pack_start(GTK_BOX(vbox), t2, FALSE, FALSE, 0);

    app.tipsBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_widget_set_margin_start(app.tipsBox, 16);
    gtk_widget_set_margin_end(app.tipsBox, 16);
    gtk_widget_set_margin_bottom(app.tipsBox, 4);
    gtk_box_pack_start(GTK_BOX(vbox), app.tipsBox, FALSE, FALSE, 0);

    GtkWidget* sep2 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    applyClass(sep2, "divider");
    gtk_box_pack_start(GTK_BOX(vbox), sep2, FALSE, FALSE, 0);

    // ── Kuchaytirish variantlari ──────────────────────────────
    GtkWidget* t3 = gtk_label_new("PAROLNI KUCHAYTIRISH");
    gtk_label_set_xalign(GTK_LABEL(t3), 0.0f);
    applyClass(t3, "section-title");
    gtk_box_pack_start(GTK_BOX(vbox), t3, FALSE, FALSE, 0);

    app.suggestionsBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_margin_start(app.suggestionsBox, 16);
    gtk_widget_set_margin_end(app.suggestionsBox, 16);
    gtk_widget_set_margin_bottom(app.suggestionsBox, 8);
    gtk_box_pack_start(GTK_BOX(vbox), app.suggestionsBox, FALSE, FALSE, 0);

    GtkWidget* sep3 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    applyClass(sep3, "divider");
    gtk_box_pack_start(GTK_BOX(vbox), sep3, FALSE, FALSE, 0);

    // ── Kuchli parollar ──────────────────────────────────────
    GtkWidget* t4 = gtk_label_new("YANGI KUCHLI PAROLLAR");
    gtk_label_set_xalign(GTK_LABEL(t4), 0.0f);
    applyClass(t4, "section-title");
    gtk_box_pack_start(GTK_BOX(vbox), t4, FALSE, FALSE, 0);

    app.generatedBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_margin_start(app.generatedBox, 16);
    gtk_widget_set_margin_end(app.generatedBox, 16);
    gtk_widget_set_margin_bottom(app.generatedBox, 16);
    gtk_box_pack_start(GTK_BOX(vbox), app.generatedBox, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(scroll), vbox);
    return scroll;
}

// ════════════════════════════════════════════════════════════════
//  Asosiy oyna
// ════════════════════════════════════════════════════════════════

static void buildWindow() {
    app.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(app.window), "ParolTest — Parol Tahlilchisi");
    gtk_window_set_default_size(GTK_WINDOW(app.window), 820, 620);
    gtk_window_set_resizable(GTK_WINDOW(app.window), TRUE);
    g_signal_connect(app.window, "destroy", G_CALLBACK(gtk_main_quit), nullptr);

    GtkWidget* mainVBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    // ── Asosiy kontent (sidebar + right panel) ────────────────
    GtkWidget* contentBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_vexpand(contentBox, TRUE);

    GtkWidget* sidebar   = buildSidebar();
    GtkWidget* mainPanel = buildMainPanel();
    gtk_widget_set_hexpand(mainPanel, TRUE);

    gtk_box_pack_start(GTK_BOX(contentBox), sidebar,    FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(contentBox), mainPanel,  TRUE,  TRUE,  0);

    // ── Status bar ────────────────────────────────────────────
    app.statusBar = gtk_statusbar_new();
    applyClass(app.statusBar, "statusbar");
    gtk_statusbar_push(GTK_STATUSBAR(app.statusBar), 0,
                       "ParolTest v2.0 | Parol kiriting...");

    gtk_box_pack_start(GTK_BOX(mainVBox), contentBox, TRUE,  TRUE,  0);
    gtk_box_pack_start(GTK_BOX(mainVBox), app.statusBar, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(app.window), mainVBox);
}

// ════════════════════════════════════════════════════════════════
//  main
// ════════════════════════════════════════════════════════════════

int main(int argc, char* argv[]) {
    gtk_init(&argc, &argv);

    // CSS yuklash
    GtkCssProvider* provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, CSS, -1, nullptr);
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    buildWindow();
    gtk_widget_show_all(app.window);
    gtk_main();
    return 0;
}