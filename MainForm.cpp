#include "MainForm.h"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <fstream>

wxBEGIN_EVENT_TABLE(MainForm, wxFrame)
EVT_PAINT(MainForm::OnPaint)
EVT_LEFT_DOWN(MainForm::OnMouseClick)
wxEND_EVENT_TABLE()

MainForm::MainForm(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600)),
    isClass1Active(false), isClass2Active(false),
    isClass3Active(false), isClass4Active(false) {

    class1CheckBox = new wxCheckBox(this, wxID_ANY, "Class 1", wxPoint(650, 50));
    class2CheckBox = new wxCheckBox(this, wxID_ANY, "Class 2", wxPoint(650, 80));
    class3CheckBox = new wxCheckBox(this, wxID_ANY, "Class 3", wxPoint(650, 110));
    class4CheckBox = new wxCheckBox(this, wxID_ANY, "Class 4", wxPoint(650, 140));
    normalizeButton = new wxButton(this, wxID_ANY, "Train", wxPoint(650, 180));

    new wxStaticText(this, wxID_ANY, "Epoch:", wxPoint(650, 230));
    epochChoice = new wxChoice(this, wxID_ANY, wxPoint(700, 230), wxSize(100, 25));
    epochChoice->Append("9");
    epochChoice->Append("99");
    epochChoice->Append("999");
    epochChoice->Append("9999");

    new wxStaticText(this, wxID_ANY, "Learning Rate:", wxPoint(650, 260));
    learningRateChoice = new wxChoice(this, wxID_ANY, wxPoint(750, 260), wxSize(100, 25));
    learningRateChoice->Append("0.1");
    learningRateChoice->Append("1");
    learningRateChoice->Append("10");
    learningRateChoice->Append("100");

    Bind(wxEVT_CHECKBOX, &MainForm::OnClass1Checked, this, class1CheckBox->GetId());
    Bind(wxEVT_CHECKBOX, &MainForm::OnClass2Checked, this, class2CheckBox->GetId());
    Bind(wxEVT_CHECKBOX, &MainForm::OnClass3Checked, this, class3CheckBox->GetId());
    Bind(wxEVT_CHECKBOX, &MainForm::OnClass4Checked, this, class4CheckBox->GetId());
    Bind(wxEVT_BUTTON, &MainForm::OnNormalize, this, normalizeButton->GetId());
}

void MainForm::OnPaint(wxPaintEvent& event) {
    wxPaintDC dc(this);
    dc.SetPen(wxPen(wxColour(0, 0, 0), 2));

    dc.DrawLine(400, 0, 400, 600);  // Y ekseni
    dc.DrawLine(0, 300, 800, 300);  // X ekseni

    DrawPoints(dc);  


    if (!weights.empty()) {
        wxPen pens[] = { wxPen(wxColour(0, 0, 255), 2), wxPen(wxColour(255, 0, 0), 2),
                         wxPen(wxColour(0, 255, 0), 2), wxPen(wxColour(255, 165, 0), 2) };

        if (class1Points.size() > 0 && class2Points.size() > 0 && class3Points.size() == 0 && class4Points.size() == 0) {
            // 2 sýnýf 
            for (int i = 0; i < 2; ++i) {
                dc.SetPen(pens[i]);
                DrawSeparatingLine(dc, i);  
            }
        }
        else {
            // 4 sýnýf 
            for (size_t i = 0; i < weights.size(); ++i) {
                if (weights[i][1] == 0 && weights[i][2] == 0) continue;

                double x1 = -400 / 50.0;
                double y1 = (-weights[i][0] - weights[i][1] * x1) / weights[i][2];

                double x2 = 400 / 50.0;
                double y2 = (-weights[i][0] - weights[i][1] * x2) / weights[i][2];

                int screenX1 = 400 + x1 * 50;
                int screenY1 = 300 - y1 * 50;
                int screenX2 = 400 + x2 * 50;
                int screenY2 = 300 - y2 * 50;

                dc.SetPen(pens[i]);
                dc.DrawLine(screenX1, screenY1, screenX2, screenY2);
            }
        }
        test(50, 50); //manuel verilen test icin ornek
    }
}

void MainForm::OnMouseClick(wxMouseEvent& event) {
    wxPoint mousePos = event.GetPosition();
    double x = (mousePos.x - 400) / 50.0;
    double y = (300 - mousePos.y) / 50.0;

    if (isClass1Active) {
        class1Points.emplace_back(x, y);
        training_outputs.push_back({ 1, 0, 0, 0 }); 
    }
    else if (isClass2Active) {
        class2Points.emplace_back(x, y);
        training_outputs.push_back({ 0, 1, 0, 0 }); 
    }
    else if (isClass3Active) {
        class3Points.emplace_back(x, y);
        training_outputs.push_back({ 0, 0, 1, 0 }); 
    }
    else if (isClass4Active) {
        class4Points.emplace_back(x, y);
        training_outputs.push_back({ 0, 0, 0, 1 }); 
    }

    Refresh();
}

void MainForm::OnClass1Checked(wxCommandEvent& event) {
    isClass1Active = class1CheckBox->IsChecked();
    isClass2Active = isClass3Active = isClass4Active = false;
    class2CheckBox->SetValue(false);
    class3CheckBox->SetValue(false);
    class4CheckBox->SetValue(false);
}

void MainForm::OnClass2Checked(wxCommandEvent& event) {
    isClass2Active = class2CheckBox->IsChecked();
    isClass1Active = isClass3Active = isClass4Active = false;
    class1CheckBox->SetValue(false);
    class3CheckBox->SetValue(false);
    class4CheckBox->SetValue(false);
}

void MainForm::OnClass3Checked(wxCommandEvent& event) {
    isClass3Active = class3CheckBox->IsChecked();
    isClass1Active = isClass2Active = isClass4Active = false;
    class1CheckBox->SetValue(false);
    class2CheckBox->SetValue(false);
    class4CheckBox->SetValue(false);
}

void MainForm::OnClass4Checked(wxCommandEvent& event) {
    isClass4Active = class4CheckBox->IsChecked();
    isClass1Active = isClass2Active = isClass3Active = false;
    class1CheckBox->SetValue(false);
    class2CheckBox->SetValue(false);
    class3CheckBox->SetValue(false);
}

void MainForm::OnNormalize(wxCommandEvent& event) {
    if (epochChoice->GetSelection() == wxNOT_FOUND || learningRateChoice->GetSelection() == wxNOT_FOUND) {
        wxMessageBox("Please select both Epoch and Learning Rate before training.", "Error", wxICON_ERROR);
        return;
    }

    wxString epochStr = epochChoice->GetStringSelection();
    wxString learningRateStr = learningRateChoice->GetStringSelection();

    int epochLimit = wxAtoi(epochStr);
    double learningRate = wxAtof(learningRateStr);

    TrainNeuralNetwork(epochLimit, learningRate);
    Refresh();
}

double MainForm::sigmoid(double x) {
    return 1.0 / (1.0 + exp(-x));      // Sigmoid fonksiyonunu hesapla
}

double MainForm::activation(double x) {
    return (x > 0 ? 1 : 0);
}

std::pair<int, double>MainForm::max_selector(const std::vector<double>& netOutputs) {
    int maxIndex = 0;
    double maxValue = netOutputs[0];

    for (int i = 1; i < netOutputs.size(); ++i) {
        if (netOutputs[i] > maxValue) {
            maxValue = netOutputs[i];
            maxIndex = i;
        }
    }

    return { maxIndex, maxValue };     // Ýndeks ve maksimum deðeri döndür
}

void MainForm::test(double x, double y) {
   
    std::vector<double> netOutputs(4, 0);
    for (int j = 0; j < 4; ++j) {
        netOutputs[j] = sigmoid(weights[j][0] + weights[j][1] * x + weights[j][2] * y);
    }
    
    int idx;
    double value;

    std::pair<int, double> result = max_selector(netOutputs);

    wxString message;
    message.Printf("The class to which the point belongs: %d", result.first+1); //Noktaya ait sýnýf
    wxMessageBox(message, "Sonuç", wxOK | wxICON_INFORMATION);
}

void MainForm::TrainNeuralNetwork(int epochLimit, double learningRate) {
    weights.clear();
    weights.resize(4, std::vector<double>(3, 0)); // [bias, weight_x, weight_y]


    for (int epoch = 0; epoch < epochLimit; ++epoch) {
        bool allClassified = true;

        for (size_t i = 0; i < training_outputs.size(); ++i) {
            // Noktanýn koordinatlarýný ve hedef çýktýlarýný al
            double x = 0, y = 0;
            if (i < class1Points.size()) {
                x = class1Points[i].first;
                y = class1Points[i].second;
            }
            else if (i < class1Points.size() + class2Points.size()) {
                x = class2Points[i - class1Points.size()].first;
                y = class2Points[i - class1Points.size()].second;
            }
            else if (i < class1Points.size() + class2Points.size() + class3Points.size()) {
                x = class3Points[i - class1Points.size() - class2Points.size()].first;
                y = class3Points[i - class1Points.size() - class2Points.size()].second;
            }
            else {
                x = class4Points[i - class1Points.size() - class2Points.size() - class3Points.size()].first;
                y = class4Points[i - class1Points.size() - class2Points.size() - class3Points.size()].second;
            }

            // Çýkýþlarý hesapla
            std::vector<double> netOutputs(4, 0);
            for (int j = 0; j < 4; ++j) {
                netOutputs[j] = sigmoid(weights[j][0] + weights[j][1] * x + weights[j][2] * y);
            }

            // Hata ve aðýrlýk güncelleme
            for (int j = 0; j < 4; ++j) {
                double error = training_outputs[i][j] - netOutputs[j];
                if (std::abs(error) > 0.01) {
                    allClassified = false;
                    weights[j][0] += learningRate * error;         // Bias güncelleme
                    weights[j][1] += learningRate * error * x;    // x aðýrlýðý güncelleme
                    weights[j][2] += learningRate * error * y;    // y aðýrlýðý güncelleme
                }
            }
        }

        if (allClassified) break;
    }

}

void MainForm::DrawPoints(wxDC& dc) {
    wxBrush brushes[] = {
        *wxBLUE_BRUSH,
        *wxRED_BRUSH,
        *wxGREEN_BRUSH,
        wxBrush(wxColour(255, 165, 0)) 
    };

    for (const auto& point : class1Points) {
        double x = point.first;
        double y = point.second;
        dc.SetBrush(brushes[0]);  // Class 1 renk
        dc.DrawCircle(400 + x * 50, 300 - y * 50, 5);
    }
    for (const auto& point : class2Points) {
        double x = point.first;
        double y = point.second;
        dc.SetBrush(brushes[1]);  // Class 2 renk
        dc.DrawCircle(400 + x * 50, 300 - y * 50, 5);
    }
    for (const auto& point : class3Points) {
        double x = point.first;
        double y = point.second;
        dc.SetBrush(brushes[2]);  // Class 3 renk
        dc.DrawCircle(400 + x * 50, 300 - y * 50, 5);
    }
    for (const auto& point : class4Points) {
        double x = point.first;
        double y = point.second;
        dc.SetBrush(brushes[3]);  // Class 4 renk
        dc.DrawCircle(400 + x * 50, 300 - y * 50, 5);
    }
}

void MainForm::DrawSeparatingLine(wxDC& dc, int classIndex) {
    if (classIndex >= 0 && classIndex < weights.size()) {
        double x1 = -400 / 50.0;
        double y1 = (-weights[classIndex][0] - weights[classIndex][1] * x1) / weights[classIndex][2];

        double x2 = 400 / 50.0;
        double y2 = (-weights[classIndex][0] - weights[classIndex][1] * x2) / weights[classIndex][2];

        int screenX1 = 400 + x1 * 50;
        int screenY1 = 300 - y1 * 50;
        int screenX2 = 400 + x2 * 50;
        int screenY2 = 300 - y2 * 50;

        wxPen pens[] = { wxPen(wxColour(0, 0, 255), 2), wxPen(wxColour(255, 0, 0), 2),
                         wxPen(wxColour(0, 255, 0), 2), wxPen(wxColour(255, 165, 0), 2) };

        dc.SetPen(pens[classIndex]);
        dc.DrawLine(screenX1, screenY1, screenX2, screenY2);
    }
}
