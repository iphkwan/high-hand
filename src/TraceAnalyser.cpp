#include "TraceAnalyser.h"

bool TraceAnalyser::InitGestureList(const char *filepath) {
    ifstream fin(filepath);
    string a, b;
    if (fin.is_open()) {
        while (fin >> a >> b) {
            gestureList.push_back(pair<string, string>(a, b));
        }
        fin.close();
        return true;
    }
    return false;
}

bool TraceAnalyser::AddCenter(Point handCenter) {
    centerTrace.push_back(handCenter);
    return true;
}

bool TraceAnalyser::AddHotspot(Point handHotspot) {
    hotspotTrace.push_back(handHotspot);
}

vector<Point> TraceAnalyser::GetTraceList() {
    return hotspotTrace;
}

bool TraceAnalyser::CleanTrace() {
    centerTrace.clear();
    hotspotTrace.clear();
    dirStr = "";
}

char TraceAnalyser::JudgeDirection(Point &last, Point &cur) {
    if (fabs(cur.x - last.x) < 1e-8) {
        if (cur.y < last.y)
            return 'D';
        return 'U';
    }
    float slope = (cur.y - last.y) / (cur.x - last.x);
    if (cur.y > last.y) {
        if (slope > 1 || slope < -1)
            return 'U';
        else if (cur.x > last.x)
            return 'R';
        return 'L';
    } else {
        if (slope > 1 || slope < -1)
            return 'D';
        else if (cur.x > last.x)
            return 'R';
        return 'L';
    }
}

int TraceAnalyser::CalEditDistance(string &tmpStr) {
    int dp[16][16];
    int len1 = dirStr.length();
    int len2 = tmpStr.length();
    if (len1 > 15 || len2 > 15)
        return -1;
    if (len1 == 0 || len2 == 0)
        return len1 + len2;
    for (int i = 0; i <= len1; i++)
        dp[i][0] = i;
    for (int i = 0; i <= len2; i++)
        dp[0][i] = i;
    for (int i = 0; i <= len1; i++)
        for (int j = 0; j <= len2; j++) {
            dp[i + 1][j + 1] = dp[i][j] + ((int)tmpStr[i] == dirStr[i] ? 0 : 1);
            dp[i + 1][j + 1] = min(dp[i + 1][j + 1], dp[i + 1][j] + 1);
            dp[i + 1][j + 1] = min(dp[i + 1][j + 1], dp[i][j + 1] + 1);
        }
    return dp[len1][len2];
}

string TraceAnalyser::PredictResult() {
    if (gestureList.size() == 0 || dirStr.length() < 3 || dirStr.length() > 10)
        //return "None";
        return dirStr;
    int max_dis = 16, p = -1, tmp;
    for (int i = 0; i < gestureList.size(); i++) {
        tmp = this->CalEditDistance(gestureList[i].first);
        if (tmp < max_dis) {
            max_dis = tmp;
            p = i;
        }
    }
    if (p != -1 && max_dis <= 3)
        return gestureList[p].second + " " + dirStr;
    //return "None";
    return dirStr;
}

int TraceAnalyser::SqrDis(Point &x, Point &y) {
    return (x.x - y.x) * (x.x - y.x) + (x.y - y.y) * (x.y - y.y);
}

bool TraceAnalyser::AnalyseTrace(Point handCenter, Point handHotspot) {
    if (centerTrace.size() == 0) {
        centerTrace.push_back(handCenter);
        hotspotTrace.push_back(handHotspot);
        lastHotspot = handHotspot;
        return true;
    }
    int d = SqrDis(handHotspot, lastHotspot);
    if (d > 100 && d < 90000) {
        //dirStr += this->JudgeDirection(hotspotTrace.back(), handHotspot);
        centerTrace.push_back(handCenter);
        hotspotTrace.push_back(handHotspot);
        if (d > 2500) {
            char c = JudgeDirection(this->lastHotspot, handHotspot);
            if (dirStr.length() == 0 || c != dirStr[ dirStr.length() - 1 ]) {
                dirStr += c;
                this->lastHotspot = handHotspot;
            }
        }
        return true;
    }
    return false;
}
