// TODO : Error Handling
// TODO : Update EV_SENT logic :DONE

#include <fstream>
#include <ostream>
#include <sstream>
#include <iostream>
#include <iterator>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <list>

using namespace std;

#define MAXSITES 9

int NSITES;
list<struct st_event> ev_list;
list<struct st_event> ev_pending;
map<pair<int, string>, struct st_event> sent_tracker;
vector<list<struct st_event>> ev_processed;
int curr_site;
vector<vector<int>> vc_clock;

enum en_event {
    EV_SENT,
    EV_RECVB,
    EV_RECVA
};

void print_clock(vector<int> vc) {
    for (auto v : vc) {
        cout << v;
    }
    cout << endl;
}

struct st_event {
    en_event ev_type;
    int associated_site_id;
    int origin_site_id;
    string msg;
    bool processed;
    vector<int> vc;

    void print_event() {
        cout << endl;
        cout << "Event type: ";
        switch(ev_type) {
            case EV_SENT:
                cout << "EV_SENT";
                break;
            case EV_RECVB:
                cout << "EV_RECVB";
                break;
            case EV_RECVA:
                cout << "EV_RECVA";
                break;
            default:
                break;
        }
        cout << endl;
        cout << "Associated site: P" << associated_site_id << endl;
        if (ev_type != EV_SENT)
            cout << "Origin site: P" << origin_site_id << endl;
        cout << "Message: " << msg << endl;
        cout << "Clock: ";
        print_clock(this->vc);
    }

    string get_vc_str() {
        string vc_str = "";
        for (auto vc : this->vc) {
            vc_str += to_string(vc);
        }
        return vc_str;
    }
};

st_event new_event(en_event ev_type, string msg, int associated_site_id, int origin_site_id = -1) {
    struct st_event tempevent;
    tempevent.ev_type = ev_type;
    tempevent.msg = msg;
    tempevent.associated_site_id = associated_site_id;
    tempevent.origin_site_id = origin_site_id;
    tempevent.processed = false;


    return tempevent;
}

void initialize(int nsites) {
    ev_processed.resize(nsites);
    vc_clock.resize(NSITES, vector<int> (NSITES,0));
}

bool is_deliverable(struct st_event &ev) {

    // check is sent
    auto it = sent_tracker.find(make_pair(ev.origin_site_id, ev.msg));
    if (it == sent_tracker.end())
        return false;

    bool deffered = false;

    if (it != sent_tracker.end()) {
        // check if deliverable
        vector<int> origin_vc = it->second.vc;
        for (int i = 0; i < NSITES; i++) {

            if (i == ev.origin_site_id) {
                if (origin_vc[i] != vc_clock[curr_site][i] + 1) {
                    deffered = true;
                    break;
                }
            }
            else if (origin_vc[i] > vc_clock[curr_site][i]) {
                deffered = true;
                break;
            }
        }
    }
    return !deffered;
}

void timestamp(struct st_event &ev) {

    switch (ev.ev_type) {
        case EV_RECVB:
            ev.vc = vc_clock[curr_site];
            // ev.processed = true;
            ev_processed[curr_site].emplace_back(ev);
            break;
        case EV_RECVA: {
            auto it = sent_tracker.find(make_pair(ev.origin_site_id, ev.msg));
            vector<int> origin_vc = it->second.vc;
            vector<int> tempvc;
            for (int i = 0; i < NSITES; i++) {
                tempvc.emplace_back(max(vc_clock[curr_site][i],origin_vc[i]));
            }
            ev.vc = tempvc;
            vc_clock[curr_site] = tempvc;
            ev.processed = true;
            ev_processed[curr_site].emplace_back(ev);
            break;
                       }
        case EV_SENT:
            vc_clock[curr_site][ev.associated_site_id]++;
            ev.vc = vc_clock[curr_site];
            ev.processed = true;
            ev_processed[curr_site].emplace_back(ev);
            break;
        default:
            break;
    }
}

void simulate2() {

    curr_site = 0;
    ev_pending = ev_list;
    bool recv_A_processed;

    while(!ev_pending.empty()) {
        for (auto ev_it = begin(ev_pending); ev_it != end(ev_pending);) {

            // & is very important
            struct st_event &ev = *ev_it;
            if (ev.associated_site_id != curr_site) {
                curr_site = ev.associated_site_id;
                recv_A_processed = true;
            }

            switch (ev.ev_type) {
                case EV_RECVB:
                    // time stamp
                    timestamp(ev);
                    // change to RECV_A
                    ev.ev_type = EV_RECVA;
                    break;
                case EV_RECVA:
                    // check if can be processed
                    // once recv_A_processed is false 
                    // don't make it true
                    if (recv_A_processed) {
                        recv_A_processed = is_deliverable(ev);
                    }
                    if (is_deliverable(ev)) {
                        timestamp(ev);
                        // remove from pending
                        ev_it = ev_pending.erase(ev_it);
                    }
                    else {
                        ev_it++;
                    }
                    break;
                case EV_SENT:
                    // if not the first event of the site and
                    // if any previous RECV_A of the same site
                    // is not processed
                    if ((!ev_processed[curr_site].empty()) &&
                        !(recv_A_processed)) {
                            // skip to next site
                            // if at end then go back
                            if (next(ev_it) == ev_pending.end()) {
                                ev_it = begin(ev_pending);
                                break;
                            }
                            // this is important else weird behaviour
                            struct st_event tempev = *ev_it;
                            while(tempev.associated_site_id == curr_site) {
                                ev_it++;
                                tempev = *ev_it;
                            }
                    }
                    // else time stamp, add to tracker
                    else {
                        timestamp(ev);
                        sent_tracker[make_pair(ev.associated_site_id, ev.msg)] = ev;
                        // remove from pending
                        ev_it = ev_pending.erase(ev_it);
                    }
                    break;
                default:
                    break;
            }
        }
    }
}

void generate_output() {

    for (int i = 0; i < NSITES; i++) {
        cout << "begin process p" + to_string(i+1) << endl;
        for (auto ev : ev_processed[i]) {
            switch (ev.ev_type) {
                case EV_SENT:
                    cout << "send " + ev.msg + " (" + ev.get_vc_str() + ")";
                    cout << endl;
                    break;
                case EV_RECVB:
                    cout << "recv_B p" + to_string(ev.origin_site_id + 1) +
                        " " + ev.msg + " (" + ev.get_vc_str() + ")";
                    cout << endl;
                    break;
                case EV_RECVA:
                    cout << "recv_A p" + to_string(ev.origin_site_id + 1) +
                        " " + ev.msg + " (" + ev.get_vc_str() + ")";
                    cout << endl;
                    break;
                default:
                    break;
            }
        }
        cout << "end process" << endl;
        if (i != NSITES - 1)
            cout << endl;
    }
}


void parse_input(istream &input) {

    string line;
    int site_count = 0;

    while (getline(input,line)) {

        istringstream iss(line);
        string word;
        iss >> word;

        if (word == "begin") {
            string site_name;
            site_count++;
        }
        else if (word == "send") {
            string msg;
            iss >> msg;
            ev_list.emplace_back(new_event(EV_SENT, msg, site_count-1));
        }
        else if (word == "recv_B") {
            string origin_site, msg;
            iss >> origin_site >> msg;
            origin_site = origin_site.substr(1);
            int origin_id = stoi(origin_site) - 1;
            ev_list.emplace_back(new_event(EV_RECVB, msg, site_count-1, origin_id));
        }
    }

    NSITES = site_count;
}

void print_ev_list(list<struct st_event> evl) {
    for (auto ev_it = begin(evl); ev_it != end(evl); ev_it++) {
        struct st_event tempev = *ev_it;
        tempev.print_event();
    }
}

int main(int argc, char *argv[])
{
    if (argc == 1) {
        cout << "Enter input (ctrl+d to terminate):" << endl;
        parse_input(cin);
        cout << endl;
    }
    else if (argc == 2) {
        ifstream file(argv[1]);
        if (!file) {
            std::cerr << "Error: Cannot open file " << argv[1] << endl;
            exit(1);
        }
        parse_input(file);
        file.close();
    }
    else {
        cout << "Usage ./bss <input-file>" << endl;
        exit(0);
    }

    initialize(NSITES);
    simulate2();
    generate_output();
}
