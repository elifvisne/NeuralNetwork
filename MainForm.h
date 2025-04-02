#pragma once

#include <wx/wx.h>
#include <vector>
#include <utility> 

class MainForm : public wxFrame
{
public:
    MainForm(const wxString& title);
    std::pair<int, double>max_selector(const std::vector<double>& netOutputs);

private:
    wxCheckBox* class1CheckBox;
    wxCheckBox* class2CheckBox;
    wxCheckBox* class3CheckBox;
    wxCheckBox* class4CheckBox;
    wxButton* normalizeButton;
    wxChoice* epochChoice;
    wxChoice* learningRateChoice;

    // Sýnýf bilgileri
    std::vector<std::pair<double, double>> class1Points;
    std::vector<std::pair<double, double>> class2Points;
    std::vector<std::pair<double, double>> class3Points;
    std::vector<std::pair<double, double>> class4Points;

    // Eðitim verileri (sýnýf çýktýlarý)
    std::vector<std::vector<int>> training_outputs;  // Her sýnýf için çýktýlar (örneðin [1, 0, 0, 0])

    // Aðýrlýklar (bias + x ve y için aðýrlýklar)
    std::vector<std::vector<double>> weights;

    // Eðitim sýrasýnda kullanýlacak fonksiyonlar
    void OnPaint(wxPaintEvent& event);
    void OnMouseClick(wxMouseEvent& event);
    void OnClass1Checked(wxCommandEvent& event);
    void OnClass2Checked(wxCommandEvent& event);
    void OnClass3Checked(wxCommandEvent& event);
    void OnClass4Checked(wxCommandEvent& event);
    void OnNormalize(wxCommandEvent& event);
    void TrainNeuralNetwork(int epochLimit, double learningRate);
    void DrawPoints(wxDC& dc);
    void DrawSeparatingLine(wxDC& dc, int classIndex);
    void test(double x, double y);


    double sigmoid(double x);
    double activation(double x);

    bool isClass1Active;
    bool isClass2Active;
    bool isClass3Active;
    bool isClass4Active;

    // Olay tablolarý
    wxDECLARE_EVENT_TABLE();
};