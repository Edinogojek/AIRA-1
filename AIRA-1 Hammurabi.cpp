#include <iostream>
#include <fstream>
#include <random>
#include <Windows.h>
#include <string>
#include <algorithm>
#include <ctime>

using namespace std;

const string saveFileName = "savegame.txt";

const int totalRounds = 10;

const int minLandPrice = 17;
const int maxLandPrice = 26;

const int minWheatPerLand = 1;
const int maxWheatPerLand = 6;

const int minWheatEatenByRatsPercent = 0;
const int maxWheatEatenByRatsPercent = 7;

const int populationPerLand = 10;
const int wheatConsumptionPerPopulation = 20;
const int starvedToDeathToGameOverPercentage = 45;

const int plagueChance = 15;

const double wheatToPlantOnLand = 0.5;

double scoreP = 0; //Среднегодовой процент умерших от голода
double scoreL = 0; //Количество акров земли на одного жителя

bool isGameOver = false;

struct GameState {
    int population = 100;
    int wheat = 2800;
    int land = 1000;
    
    int round = 0;
    int populationStarved = 0;
    int populationArrived = 0;
    bool isPlague = false;

    int wheatHarvested = 0;
    int wheatHarvestedPerLand = 0;
    int wheatEatenByRats = 0;
    int landPrice = 0;

    double totalPercentStarved = 0;
};

void SaveGame(const GameState& gs) {
    ofstream outFile(saveFileName);
    outFile << gs.population << "\n"
        << gs.wheat << "\n"
        << gs.land << "\n"
        << gs.round << "\n"
        << gs.populationStarved << "\n"
        << gs.populationArrived << "\n"
        << gs.isPlague << "\n"
        << gs.wheatHarvested << "\n"
        << gs.wheatHarvestedPerLand << "\n"
        << gs.wheatEatenByRats << "\n"
        << gs.landPrice << "\n"
        << gs.totalPercentStarved << "\n";

    outFile.close();
}

bool LoadGame(GameState& gs) {
    ifstream inFile(saveFileName);
    if (!(inFile >> gs.population
        >> gs.wheat
        >> gs.land
        >> gs.round
        >> gs.populationStarved
        >> gs.populationArrived
        >> gs.isPlague
        >> gs.wheatHarvested
        >> gs.wheatHarvestedPerLand
        >> gs.wheatEatenByRats
        >> gs.landPrice
        >> gs.totalPercentStarved)) {
        inFile.close();
        return false;
    }

    inFile.close();
    return true;
}

void ShowStats(GameState& gs) {
    cout << "Мой повелитель, соизволь поведать тебе" << endl;
    cout << "в году " << gs.round + 1 << " твоего высочайшего правления" << endl << endl;
    if (gs.populationStarved > 0) cout << gs.populationStarved << " человек умерли с голоду, и ";
    if(gs.populationArrived > 0)cout << gs.populationArrived << " человек прибыли в наш великий город;" << endl;
    if (gs.isPlague) {
        cout << "Чума уничтожила половину населения;" << endl;
    }
    cout << "Население города сейчас составляет " << gs.population << " человек;" << endl;
    if(gs.wheatHarvested > 0) cout << "Мы собрали " << gs.wheatHarvested << " бушелей пшеницы, по " << gs.wheatHarvestedPerLand << " бушеля с акра;" << endl;
    cout << "Крысы истребили " << gs.wheatEatenByRats << " бушелей пшеницы, оставив " << gs.wheat << " бушеля в амбарах" << endl;
    cout << "Город сейчас занимает " << gs.land << " акров;" << endl;
    cout << "1 акр земли стоит сейчас " << gs.landPrice << " бушель." << endl << endl;
}

bool isInputCorrect(const std::string& s, int& out) {
    if (s.empty()) return false;

    if (!all_of(s.begin(), s.end(), [](unsigned char c) { return std::isdigit(c); })) {
        return false;
    }

    try {
        out = stoi(s);
        return true;
    }
    catch (...) {
        return false;
    }
}

int ReadInput() {
    string input;
    int result = 0;

    while (true) 
    {
        std::getline(cin, input);
        if (isInputCorrect(input, result)) {
            return result;
        }
        else {
            cout << "Повелитель, я вас не понял. изволишь повторить? " << endl;
        }
    }
}

int CalculateWheatToSpend(GameState& gs, int landToBuy, int wheatToEat, int landToPlant) {
    return wheatToEat + landToBuy * gs.landPrice + landToPlant * wheatToPlantOnLand;
}

bool CanAffordDescicions(GameState& gs, int landToBuy, int wheatToEat, int landToPlant) {
    int wheatToSpendTotal = CalculateWheatToSpend(gs, landToBuy, wheatToEat, landToPlant);
    return (gs.wheat >= wheatToSpendTotal) &&
        (gs.population * populationPerLand >= landToPlant) &&
        (gs.land + landToBuy >= landToPlant);
}

void GameOver() {
    string FinalText;
    if (scoreP > 33 && scoreL < 7) FinalText = "Из-за вашей некомпетентности в\nуправлении, народ устроил бунт, и изгнал вас их города.Теперь вы\nвынуждены влачить жалкое существование в изгнании";
    else if (scoreP > 10 && scoreL < 9) FinalText = "Вы правили железной\nрукой, подобно Нерону и Ивану Грозному.Народ вздохнул с\nоблегчением, и никто больше не желает видеть вас правителем";
    else if (scoreP > 3 && scoreL < 10) FinalText = "Вы справились вполне неплохо, у вас,\nконечно, есть недоброжелатели, но многие хотели бы увидеть вас во\nглаве города снова";
    else FinalText = "Фантастика! Карл Великий,\nДизраэли и Джефферсон вместе не справились бы лучше";

    cout << FinalText;
    system("pause");
}

void PlayRound(GameState& gs) {
    if (gs.landPrice == 0) {
        gs.landPrice = minLandPrice + rand() % (maxLandPrice - minLandPrice + 1);
    }
    SaveGame(gs);

    ShowStats(gs);
    gs.isPlague = false;

    cout << "Что пожелаешь, повелитель?" << endl;

    int landToBuy;
    int wheatToEat;
    int landToPlant;

    while (true) {
        cout << "Сколько акров земли повелеваешь купить? ";
        landToBuy = ReadInput();

        cout << "Сколько бушелей пшеницы повелеваешь съесть? ";
        wheatToEat = ReadInput();

        cout << "Сколько акров земли повелеваешь засеять? ";
        landToPlant = ReadInput();

        if (CanAffordDescicions(gs, landToBuy, wheatToEat, landToPlant)) {     
            break;
        }
        else {
            cout << "О, повелитель, пощади нас! У нас только " <<
                gs.population << " человек, " <<
                gs.wheat << " бушелей пшеницы и " <<
                gs.land << " акров земли!" << endl;
        }
    }
    gs.wheat -= CalculateWheatToSpend(gs, landToBuy, wheatToEat, landToPlant);
    gs.land += landToBuy;
    gs.wheatHarvestedPerLand = minWheatPerLand + rand() % (maxWheatPerLand - minWheatPerLand + 1);

    gs.wheatHarvested = landToPlant * gs.wheatHarvestedPerLand;
    gs.wheat += gs.wheatHarvested;

    int wheatEatenByRatsPercent = minWheatEatenByRatsPercent + rand() % (maxWheatEatenByRatsPercent - minWheatEatenByRatsPercent + 1);
    gs.wheatEatenByRats = wheatEatenByRatsPercent * gs.wheat / 100;
    gs.wheat -= gs.wheatEatenByRats;

    if (wheatToEat >= gs.population * wheatConsumptionPerPopulation) {
        gs.populationStarved = 0;
    }
    else {
        gs.populationStarved = gs.population - wheatToEat / wheatConsumptionPerPopulation;
    }
    double roundPercentStarved = (static_cast<double>(gs.populationStarved) / gs.population) * 100.0;
    gs.totalPercentStarved += roundPercentStarved;
    if (roundPercentStarved >= starvedToDeathToGameOverPercentage)
    {
        scoreL = 0;
        scoreP = 100;
        isGameOver = true;
        GameOver();
        return;
    }
    gs.population -= gs.populationStarved;

    gs.populationArrived = gs.populationStarved / 2 + (5 - gs.wheatHarvestedPerLand) * gs.wheat / 600 + 1;
    gs.populationArrived = clamp(gs.populationArrived, 0, 50);
    gs.population += gs.populationArrived;

    int chanceValue = rand() % 100;
    if (chanceValue < plagueChance) {
        gs.isPlague = true;
        gs.population /= 2;
    }

    if (gs.population <= 0) {
        scoreL = 0;
        scoreP = 100;
        isGameOver = true;
        GameOver();
        return;
    }
    gs.landPrice = 0;
}

int main()
{
    srand(static_cast<unsigned int>(time(0)));

    setlocale(LC_ALL, "Russian");
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    GameState gs;

    ifstream checkFile(saveFileName);
    if (checkFile.good()) {
        checkFile.close();
        cout << "Повелитель, найдена летопись вашего прошлого правления. Продолжить? (1 - Да, 0 - Новая игра): ";
        int choice = ReadInput();
        if (choice == 1) {
            if (LoadGame(gs)) {
                cout << "Продолжаем ваше правление!" << endl << endl;
            }
            else {
                cout << "Не удалось прочитать летопись. Начинаем заново." << endl << endl;
                gs = GameState();
            }
        }
    }

    for (; gs.round < totalRounds; gs.round++) {
        if (isGameOver) return 0;
        PlayRound(gs);
    }
    scoreP = gs.totalPercentStarved / totalRounds;
    scoreL = static_cast<double>(gs.land) / gs.population;
    GameOver();
    remove(saveFileName.c_str());

    return 0;
}
