namespace cpp OXT

service SSEService {
    list<string> search(1:string stag, 2:list<string> xtoken, 3:string logPath),
}