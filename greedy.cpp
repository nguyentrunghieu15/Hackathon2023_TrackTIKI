/* Problem */
#include <bits/stdc++.h>

using namespace std;

#define MAX 1002
#define PARA_1 1e9
#define PARA_2 1e6
#define PARA_3 1e3
#define findID(a, N) ((a % N) == 0 ? N : a % N)

typedef struct truck
{
    int ID;
    int p;     // ID của Hub xuất phát và kết thúc
    int start; // thời điểm xe bắt đầu làm việc
    int end;   // thời điểm xe kết thúc làm việc
    float cap; // tải trọng của xe
    float vol; // Thể tích thùng xe
    float vel; // Vận tốc trung bình của xe
} truck;

typedef struct pakage
{
    int ID;
    int startHub; // ID của Hub nhận nơi gói hàng được lấy
    int endHub;   // ID của Hub trả nơi gói hàng được trả
    float m;      // khoi luong goi hang
    float vol;    // Thể tích gói hàng (m3
    int sp;       // Thời gian bốc hàng lên xe tại Hub nhận (giây)
    int sd;       // hời gian dỡ hàng xuống xe tại Hub trả (giây)
    int ep;       // thời điểm sớm nhất lấy hàng tại Hub nhận
    int ip;       // thời điểm muộn nhất lấy hàng tại Hub nhận
    int ed;       // thời điểm sớm nhất giao hàng tại Hub trả
    int id;       // thời điểm muộn nhất giao hàng tại Hub trả
} pakage;

typedef struct hisHub
{
    int ID;
    vector<int> pakage;
} hisHub;

int convertTimeToSecond(string time)
{
    return ((time[0] - '0') * 10 + time[1] - '0') * 60 * 60 + ((time[3] - '0') * 10 + time[4] - '0') * 60 + ((time[6] - '0') * 10 + time[7] - '0');
}

string convert2digitToString(int t)
{
    string temp = "00";
    temp[1] = '0' + t % 10;
    t /= 10;
    temp[0] = '0' + t % 10;
    return temp;
}

string convertSecondToTime(int second)
{
    string temp = "";
    int s = second % 60;
    second /= 60;
    int m = second % 60;
    second /= 60;
    int h = second % 60;
    temp += convert2digitToString(h) + ":" + convert2digitToString(m) + ":" + convert2digitToString(s);
    return temp;
}

int garp[MAX][MAX];
int M, N, K;
vector<truck> trucks(MAX);
vector<pakage> pakages(MAX);
vector<int> his[MAX];
vector<bool> served(MAX, false);

float avgMPakage = 0;
float avgVolPakage = 0;

float evalTruck(truck t)
{
    float n1 = t.cap / avgMPakage;
    float n2 = t.vol / avgVolPakage;
    if (n1 * n2 == 0)
        return 0;
    return t.vel * (t.end - t.start) * n1 * n1 * n2 * n2 * (2 / (1 / n1 + 1 / n2)) / 1000000;
}

int evalTimeToGo(int s, float vel)
{
    float t = 1.0 * s / vel * 60 * 60;
    return (int)t + 1;
}

int evalTimeToReceived(int pakage)
{
    if (pakage > N)
    {
        if (pakage == 2 * N)
        {
            return pakages[N].sd;
        }
        return pakages[pakage % N].sd;
    }
    return pakages[pakage].sp;
}

int evalEarlyTimeToRecieve(int cameTime, int pakage)
{
    if (pakage > N)
    {
        if (cameTime < pakages[findID(pakage, N)].ed)
        {
            cameTime = pakages[findID(pakage, N)].ed;
        }
    }
    else
    {
        if (cameTime < pakages[findID(pakage, N)].ep)
        {
            cameTime = pakages[findID(pakage, N)].ep;
        }
    }
    return cameTime;
}

void printTimelineOfTruck(vector<int> his, int ID)
{
    if (his.empty())
    {
        cout << 1 << endl;
        cout << trucks[ID].p << " " << 0 << " " << trucks[ID].start << " " << trucks[ID].start << endl;
        return;
    }
    int u = 0;

    queue<hisHub> milestone;
    for (int &i : his)
    {
        if (milestone.empty())
        {
            u++;
            milestone.push({pakages[findID(i, N)].startHub, {i}});
            continue;
        }
        if (i <= N && pakages[findID(i, N)].startHub == milestone.back().ID)
        {
            milestone.back().pakage.push_back(i);
            continue;
        }
        if (i > N && pakages[findID(i, N)].endHub == milestone.back().ID)
        {
            milestone.back().pakage.push_back(i);
            continue;
        }
        if (i <= N)
        {
            milestone.push({pakages[findID(i, N)].startHub, {i}});
        }
        else
        {
            milestone.push({pakages[findID(i, N)].endHub, {i}});
        }
        u++;
    }
    if (milestone.front().ID != trucks[ID].p)
    {
        u++;
    }
    if (milestone.back().ID != trucks[ID].p)
    {
        u++;
    }
    int curTime = trucks[ID].start;
    int curHub = trucks[ID].p;
    cout << u << endl;
    if (milestone.front().ID != trucks[ID].p)
    {
        cout << trucks[ID].p << " " << 0 << " " << convertSecondToTime(trucks[ID].start) << " " << convertSecondToTime(trucks[ID].start) << endl;
    }
    while (!milestone.empty())
    {
        hisHub temp = milestone.front();
        milestone.pop();
        curTime += evalTimeToGo(garp[curHub][temp.ID], trucks[ID].vel);
        curHub = temp.ID;
        int cameTime = evalEarlyTimeToRecieve(curTime, temp.pakage[0]);
        vector<pair<int, int>> extract;
        for (int &i : temp.pakage)
        {
            curTime = evalEarlyTimeToRecieve(curTime, i);
            extract.push_back({findID(i, N), curTime});
            curTime += evalTimeToReceived(i);
        }
        cout << temp.ID << " " << temp.pakage.size() << " " << convertSecondToTime(cameTime) << " " << convertSecondToTime(curTime) << endl;
        for (pair<int, int> &i : extract)
        {
            cout << i.first << " " << convertSecondToTime(i.second) << endl;
        }
    }
    if (curHub != trucks[ID].p)
    {
        curTime += evalTimeToGo(garp[curHub][trucks[ID].p], trucks[ID].vel);
        cout << trucks[ID].p << " " << 0 << " " << convertSecondToTime(curTime) << " " << convertSecondToTime(curTime) << endl;
    }
}

void outPut()
{
    for (int i = 1; i <= K; i++)
    {
        printTimelineOfTruck(his[i], i);
    }
}

void evalScore()
{
    int numPackageServed = 0;
    int numTruck = 0;
    int sumTime = 0;
    for (int i = 1; i <= N; i++)
    {
        numPackageServed += served[i];
    }
    for (int i = 1; i <= K; i++)
    {
        if (!his[i].empty())
        {
            numTruck++;
            sumTime += (trucks[i].end - trucks[i].start);
        }
    }
    cout << "Package: " << numPackageServed << endl;
    cout << "Truck: " << numTruck << endl;
    cout << "Score: " << (long long)(PARA_1 * numPackageServed / N + PARA_2 * numTruck / K + 1.0 / PARA_3 * sumTime) << endl;
}

void clear(int K, int N)
{
    for (int i = 1; i <= K; i++)
    {
        his[i].clear();
    }
    for (int i = 1; i <= N; i++)
    {
        served[i] = false;
    }
}

int findHubOfPackage(int package)
{
    return package > N ? pakages[findID(package, N)].endHub : pakages[findID(package, N)].startHub;
}

namespace greedy
{
    // Chi phuc vu 1 don hang tai 1 thoi diem
    void slove(int truck)
    {
        int curTime = trucks[truck].start;
        int curHub = trucks[truck].p;
        int vel = trucks[truck].vel;
        while (true)
        {
            int minTime = INT_MAX;
            int idPakage = 0;
            for (pakage &i : pakages)
            {
                if (served[i.ID])
                    continue;
                if (i.m > trucks[truck].cap || i.vol > trucks[truck].vol)
                    continue;
                int temp = curTime + evalTimeToGo(garp[curHub][i.startHub], vel);
                if (temp > i.ip)
                    continue;
                if (temp < i.ep)
                    temp = i.ep;
                temp += i.sp + evalTimeToGo(garp[i.startHub][i.endHub], vel);
                if (temp > i.id)
                    continue;
                if (temp < i.ed)
                    temp = i.ed;
                temp += i.sd;
                if (temp + evalTimeToGo(garp[i.endHub][trucks[truck].p], vel) > trucks[truck].end)
                    continue;
                if (temp < minTime)
                {
                    minTime = temp;
                    idPakage = i.ID;
                }
            }
            if (idPakage == 0)
                break;
            else
            {
                curTime = minTime;
                curHub = pakages[idPakage].endHub;
                served[idPakage] = true;
                his[trucks[truck].ID].push_back(idPakage);
                his[trucks[truck].ID].push_back(idPakage + N);
            }
        }
    }
}

int main(int argc, char const *argv[])
{
    ios_base::sync_with_stdio(false);
    std::cin.tie(0);
    std::cout.tie(0);

    // char filename[20] = "test";
    // strcat(filename, argv[1]);
    // strcat(filename, ".inp");
    // freopen(filename, "r", stdin);

    freopen("data.inp", "r", stdin);

    // Input data
    cin >> M;
    for (int i = 1; i <= M; i++)
    {
        for (int j = 1; j <= M; j++)
        {
            cin >> garp[i][j];
        }
    }

    cin >> K;

    for (int i = 1; i <= K; i++)
    {
        int p;
        string start;
        string end;
        float cap;
        float vol;
        float vel;
        cin >> p >> start >> end >> cap >> vol >> vel;
        trucks[i] = {i, p, convertTimeToSecond(start), convertTimeToSecond(end), cap, vol, vel};
    }

    cin >> N;
    for (int i = 1; i <= N; i++)
    {
        int startHub; // ID của Hub nhận nơi gói hàng được lấy
        int endHub;   // ID của Hub trả nơi gói hàng được trả
        float m;      // khoi luong goi hang
        float vol;    // Thể tích gói hàng (m3
        int sp;       // Thời gian bốc hàng lên xe tại Hub nhận (giây)
        int sd;       // hời gian dỡ hàng xuống xe tại Hub trả (giây)
        string ep;    // thời điểm sớm nhất lấy hàng tại Hub nhận
        string ip;    // thời điểm muộn nhất lấy hàng tại Hub nhận
        string ed;    // thời điểm sớm nhất giao hàng tại Hub trả
        string id;    // thời điểm muộn nhất giao hàng tại Hub trả
        cin >> startHub >> endHub >> m >> vol >> sp >> sd >> ep >> ip >> ed >> id;
        avgMPakage += m;
        avgVolPakage += vol;
        pakages[i] = {i,
                      startHub,
                      endHub,
                      m,
                      vol,
                      sp,
                      sd,
                      convertTimeToSecond(ep),
                      convertTimeToSecond(ip),
                      convertTimeToSecond(ed),
                      convertTimeToSecond(id)};
    };

    avgMPakage /= N;
    avgVolPakage /= N;

    vector<int> sortedTrucks(K);
    std::iota(sortedTrucks.begin(), sortedTrucks.end(), 1);

    // sort truck
    sort(sortedTrucks.begin(), sortedTrucks.end(), [](int a, int b)
         { return evalTruck(trucks[a]) > evalTruck(trucks[b]); });

    for (int &i : sortedTrucks)
    {
        // greedy::slove(i);
    }

    // evalScore();
    // outPut();
    return 0;
}