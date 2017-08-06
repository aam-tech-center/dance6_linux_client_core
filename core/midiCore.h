/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   midiCore.h
 * Author: Jack.L
 *
 * Created on 2017年7月15日, 上午12:52
 */

#ifndef MIDICORE_H
#define MIDICORE_H

#define MIDI_DEBUG_REPORT

////////
#include <iostream>
#include <fstream>
#include <vector>
#include <list>
using namespace std;

#include "memPool.h"

////////
#ifndef MIDI_BYTE
#define MIDI_BYTE unsigned char
#endif

////////
enum ENUM_MIDI_EVENT
{
    ////////
    EME_CLOSENOTE = 0,
    EME_OPENNOTE,
    EME_TOUCHBOARD,
    EME_SWITCHCTRL,
    EME_CHANGEPROG,
    EME_CHANNELCTRL,
    EME_GLIDE,
    EME_META,    
    
    ////////
    EME_UNKNOWN,
};

enum ENUM_MIDI_META_STATUS
{
    EMMS_TRACKSEQ    = 0x00,
    EMMS_MUSICINFO   = 0x01,
    EMMS_COPYRIGHT   = 0x02,
    EMMS_TITLE       = 0x03,
    EMMS_INST_NAME   = 0x04,
    EMMS_LYRIC       = 0x05,
    EMMS_MARK        = 0x06,
    EMMS_START       = 0x07,
    EMMS_TRACKSTART  = 0x21,
    EMMS_TRACKEND    = 0x2F,
    EMMS_SPEED       = 0x51,
    EMMS_BEAT        = 0x58,
    EMMS_TUNE        = 0x59,
    EMMS_SORT        = 0x7F,
    
    ////////
    EMMS_UNKNOWN     = 0xFF,
};

enum ENUM_MIDI_VOICE_HEIGHT
{
    EMVH_C = 0,
    EMVH_Cx,
    EMVH_D,
    EMVH_Dx,
    EMVH_E,
    EMVH_F,
    EMVH_Fx,
    EMVH_G,
    EMVH_Gx,
    EMVH_A,
    EMVH_Ax,
    EMVN_B            
};

////////
struct MIDI_META
{
    MIDI_BYTE             _data[256];    
    ENUM_MIDI_META_STATUS _status;
};

struct MIDI_EVENT
{
    int              _delay;          
    ENUM_MIDI_EVENT  _event;
    MIDI_BYTE        _channel;     
};

typedef std::vector<MIDI_EVENT*> MIDI_EVENT_ARRAY;
#define MAX_MIDI_TRACK 1024

////////
struct MIDI_TRACK
{
    MIDI_EVENT_ARRAY _eventArray;        
    
    ~MIDI_TRACK();
    void push_back(MIDI_EVENT* event);
};


////////
struct MIDI_EVENT_NOTE:
public MIDI_EVENT
{
    MIDI_BYTE _bNote; //按键
    MIDI_BYTE _bVel;  //力度
    
    const char* getNoteName();    
};

struct MIDI_EVENT_SWITCH:
public MIDI_EVENT
{
    MIDI_BYTE _bReg;  //寄存器
    MIDI_BYTE _bVal;  //值
};

struct MIDI_EVENT_INSTRUMENT:
public MIDI_EVENT
{
    MIDI_BYTE _instrument;    
};

struct MIDI_EVENT_CHANNEL:
public MIDI_EVENT
{
    MIDI_BYTE _channel;
};

struct MIDI_EVENT_GLIDE:
public MIDI_EVENT
{
    MIDI_BYTE _glide;
};

struct MIDI_EVENT_META:
public MIDI_EVENT
{
    MIDI_META  _meta;
};

////////


////////
class MIDI_BUFF;
class point_ctrl;

////////
class midiCore 
{
protected:
    
    ////////
    unsigned short m_type;
    unsigned short m_tracks;
    unsigned short m_tickcount;
    
    int*           m_metaSpeedDefault;
    int*           m_metaBeatDefault;
    
    ifstream       m_file;
    
    ////////
    MIDI_TRACK*    m_trackArray[MAX_MIDI_TRACK];
    
    ////////
    void processTrack(MIDI_BUFF* _buff, unsigned long long _buffsize, int _trackIndex);
    void processMeta(point_ctrl* _point, int _trackIndex, MIDI_BYTE _metaType, MIDI_BYTE _metaBytes);
    
public:
    midiCore();
    virtual ~midiCore();
    
    bool load(const char* _filePath);
    
    int  getTrackCount();
    unsigned long getTrackLength(int index);
    MIDI_TRACK*   getTrack(int index);
    
    void endLoad();
    void process();
    
    int getSyncType();
    int getTotalTickCount();
    
    int* getMetaSpeedDefault();
    int* getMetaBeatDefault();
    
private:

};

////dispatch track to nodelist
#define MAX_MIDI_EVENT_IN_NODE 16
struct MIDI_NODE
{
    int         _delay;
    MIDI_EVENT* _event[MAX_MIDI_EVENT_IN_NODE];
};

typedef std::vector<MIDI_NODE*> MIDI_NODE_ARRAY;

////////
class MidiSystem
{
protected:
    MemPool<MIDI_NODE>  m_nodeAlloc;    
    MIDI_NODE_ARRAY     m_nodeArray;
    int                 m_maxEvent;    
    
    MIDI_NODE*  createNode();
    void pushEventToNode(MIDI_NODE*& node, MIDI_EVENT*& event);
    
public:
    
    MidiSystem();
    ~MidiSystem();
    
    bool processTrack(MIDI_TRACK* track);  
    void clean();
    
    int  getMaxEvent();
	
    MIDI_NODE_ARRAY& getList();
};

////////
class MidiNoteSystem
{
public:
    struct NOTE
    {            
        unsigned int  _start;
        unsigned int  _end;
        unsigned int  _last;
        unsigned int  _beat;
        
        unsigned int* _minBeat;
                
        MIDI_BYTE _bNote; //按键
        MIDI_BYTE _bVel;  //力度     
        
        ENUM_MIDI_VOICE_HEIGHT getHeight();
        const char* getHeightName();
        int getLevel();
        int getBeatPercent();
    };
    
    struct NOTE_NODE
    {
        NOTE* _note[MAX_MIDI_EVENT_IN_NODE];
    };
    
    typedef MemPool<NOTE>           NOTE_ALLOC;
    typedef MemPool<NOTE_NODE>      NOTE_NODE_ALLOC;
    
    typedef std::vector<NOTE*>      NOTE_ARRAY;
    typedef std::vector<NOTE_NODE*> NOTE_NODE_ARRAY;
    
    unsigned int                    m_minTick;
    
protected:
    NOTE_ALLOC      m_noteAlloc;
    NOTE_NODE_ALLOC m_noteNodeAlloc;
    
    NOTE_NODE_ARRAY m_noteNodeArray;
    
public:
    
    MidiNoteSystem(MIDI_NODE_ARRAY& _array);
    ~MidiNoteSystem();
    
    NOTE_NODE_ARRAY& getNoteArray();
    
};

#endif /* MIDICORE_H */

