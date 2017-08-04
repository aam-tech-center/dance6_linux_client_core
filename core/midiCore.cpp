/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   midiCore.cpp
 * Author: Jack.L
 * 
 * Created on 2017年7月15日, 上午12:52
 */

#include "midiCore.h"
#include <string.h>

////////
#ifdef MIDI_DEBUG_REPORT
#define DEBUG_REPORT_SWITCH true
#else if
#define DEBUG_REPORT_SWITCH false
#endif

////////
#define DEBUG_REPORT(x) if(DEBUG_REPORT_SWITCH){std::cout<<x<<std::endl;};

////////
#define MIDI_ENT_CLOSENOTE   0x80    //0x10000000
#define MIDI_ENT_OPENNOTE    0x90    //0x10010000
#define MIDI_ENT_TOUCHBOARD  0xA0    //0x10100000
#define MIDI_ENT_SWITCHCTRL  0xB0    //0x10110000
#define MIDI_ENT_CHANGEPROG  0xC0    //0x11000000
#define MIDI_ENT_CHANNELCTRL 0xD0    //0x11010000
#define MIDI_ENT_GLIDE       0xE0    //0x11100000
#define MIDI_ENT_META        0xF0    //0x11110000

#define MIDI_META_TRACKSEQ   0x00
#define MIDI_META_MUSICINFO  0x01
#define MIDI_META_COPYRIGHT  0x02
#define MIDI_META_MUSICTITLE 0x03
#define MIDI_META_INST_NAME  0x04
#define MIDI_META_LYRIC      0x05
#define MIDI_META_MARK       0x06
#define MIDI_META_START      0x07
#define MIDI_META_TRACKSTART 0x21
#define MIDI_META_TRACKEND   0x2F
#define MIDI_META_SPEED      0x51
#define MIDI_META_BEAT       0x58
#define MIDI_META_TUNE       0x59
#define MIDI_META_SORT       0x7F

////////
char* NoteToString(MIDI_BYTE bNote)
{    
    char szBuf[16];//顶多5字符
    memset(szBuf, 0, sizeof(szBuf));
  
    switch(bNote % 12)
    {
    case 0:
        sprintf(szBuf,"C%d",(int)(bNote / 12)-2);
        break;
    case 1:
        sprintf(szBuf,"C%d#",(int)(bNote / 12)-2);
        break;
    case 2:
        sprintf(szBuf,"D%d",(int)(bNote / 12)-2);
        break;
    case 3:
        sprintf(szBuf,"D%d#",(int)(bNote / 12)-2);
        break;
    case 4:
        sprintf(szBuf,"E%d",(int)(bNote / 12)-2);
        break;
    case 5:
        sprintf(szBuf,"F%d",(int)(bNote / 12)-2);
        break;
    case 6:
        sprintf(szBuf,"F%d#",(int)(bNote / 12)-2);
        break;
    case 7:
        sprintf(szBuf,"G%d",(int)(bNote / 12)-2);
        break;
    case 8:
        sprintf(szBuf,"G%d#",(int)(bNote / 12)-2);
        break;
    case 9:
        sprintf(szBuf,"A%d",(int)(bNote / 12)-2);
        break;
    case 10:
        sprintf(szBuf,"A%d#",(int)(bNote / 12)-2);
        break;
    case 11:
        sprintf(szBuf,"B%d",(int)(bNote / 12)-2);
        break;
    }
    return szBuf;
}

////////
const char* MIDI_EVENT_NOTE::getNoteName()
{
    const char* _notename = NoteToString(_bNote);    
    return _notename;
}

MIDI_TRACK::~MIDI_TRACK()
{
    for( unsigned int i=0; i<_eventArray.size(); i++ )
    { 
        MIDI_EVENT*& _me = _eventArray[i];
                    
        if( _me )        
        {        
            delete _me;            
            _eventArray[i] = NULL;            
        }        
    }      
}

void MIDI_TRACK::push_back(MIDI_EVENT* event)
{
    _eventArray.push_back(event);
}

////////
class MIDI_BUFF
{
#define MAX_MIDI_BUFF 1024 * 4
protected:
    unsigned char        m_tempBuff[MAX_MIDI_BUFF];
    unsigned char*       m_buff;
    const unsigned long  m_buffsize;

public:    
    MIDI_BUFF(unsigned long _size):
    m_buffsize(_size),
    m_buff((_size>MAX_MIDI_BUFF)?new unsigned char[_size]:m_tempBuff)
    {
        memset(m_buff, 0, m_buffsize);
    }
    
    ~MIDI_BUFF()
    {
        if( m_buffsize > MAX_MIDI_BUFF )
        {
            delete[] m_buff;            
        }
    }
    
    unsigned char*& buff()
    {
        return m_buff;
    }
};

////////
class point_ctrl
{
protected:
    unsigned char*      m_point;

    const unsigned long m_max;
    unsigned long       m_index;
    
public:
    
    point_ctrl(MIDI_BYTE* _p, unsigned long _max):
    m_point(_p),
    m_index(0),
    m_max(_max)
    {
        
    }
    
    bool next(int val=1)
    {
        if( m_index + val < m_max )
        {
            m_point += val;
            m_index += val;            
            return true;
        }
        else
        {
            return false;
        }
    }
    
    void prev()
    {
        m_point--;
        m_index -= 1;
    }
    
    MIDI_BYTE* point()
    {
        return m_point;
    }
    
    MIDI_BYTE value()
    {
        return *m_point;                
    }
    
    unsigned long pos()
    {
        return m_index;
    }
    
    unsigned long last()
    {
        return m_max - m_index;
    }
    
    bool checkEnd(int _size)
    {
        return m_index + _size < m_max;
    }
};

////////
unsigned long takeWord(MIDI_BYTE _byte[2])
{
    unsigned long _value =
    _byte[0] * 256 + _byte[1];
    
    return _value;
}

unsigned long takeDword(MIDI_BYTE _byte[4])
{
    unsigned long _value =
    _byte[0] * 256 * 256 * 256 + _byte[1] * 256 * 256 + _byte[2] * 256 + _byte[3];
    
    return _value;
}

int takeReadByte(MIDI_BYTE _byte[4], int& _readcount)
{    
    int _value = 0;
    _readcount = 0;
    
    for( int i=0; i<4; i++ )
    {
        _readcount += 1;      
        
        const unsigned char _cbyte = _byte[i];
        
        if( _value != 0 )
        {
            _value = (_value<<7)|(_cbyte & 0x7F);
        }
        else
        {
            _value = _cbyte & 0x7F;
        }
                    
        ////////
        if( !(_cbyte & 0x80) )
        {
            break;
        }   
    }    
    
    return _value;
}

////////
midiCore::midiCore() {
     
    for( int i=0; i<MAX_MIDI_TRACK; i++ )
    {
        m_trackArray[i] = NULL;
    }
}

midiCore::~midiCore() {
}

struct MtrkHeader
{
    char MidiId[4];
    long length;
    int  format;
    int  tranknum;
    int  division;
};

bool compare_header_string2(const MIDI_BYTE _str1[2], const MIDI_BYTE _str2[2])
{
    bool _check = 
        _str1[0] == _str2[0] &&
        _str1[1] == _str2[1] 
        ;    
        
    return _check;
}

bool compare_header_string4(const MIDI_BYTE _str1[4], const MIDI_BYTE _str2[4])
{
    bool _check = 
        _str1[0] == _str2[0] &&
        _str1[1] == _str2[1] &&
        _str1[2] == _str2[2] &&
        _str1[3] == _str2[3]
        ;    
        
    return _check;
}

int midiCore::getSyncType()
{
    return (int)m_type;
};

int midiCore::getTotalTickCount()
{
    return (int)m_tickcount;
};


bool midiCore::load(const char* _filePath)
{
    bool _check = false;
    
    ////////
    DEBUG_REPORT( _filePath );
    
    ////////    
    m_file.open(_filePath, ios::binary|ios::in);       

    //header
    MIDI_BYTE _midiHeader[4];
    m_file.read((char*)_midiHeader, sizeof(_midiHeader));
    
    const MIDI_BYTE _MTHD[] = {'M','T','h','d'};

    if( compare_header_string4(_midiHeader, _MTHD) )
    {
        ////////
        _check = true;        
    }
    
    if( _check )
    {
        MIDI_BYTE _value[4];
        m_file.read((char*)_value, sizeof(_value));        
        
        const MIDI_BYTE _flag[4] = {0x00,0x00,0x00,0x06};
        if( compare_header_string4(_value, _flag) )
        {
            
        }
        else
        {
            _check = false;            
        }
    }
    
    if( _check )
    {
        unsigned char _temp[2];
        
        m_file.read((char*)_temp, sizeof(_temp));
        
        const MIDI_BYTE _flag1[3][2] =
        {
            {0x00,0x00},
            {0x00,0x01},
            {0x00,0x02},
        };
        
        if( compare_header_string2(_temp, _flag1[0]) )
        {
            m_type = 0;
        }
        else if( compare_header_string2(_temp, _flag1[1]) )
        {
            m_type = 1;            
        }
        else if( compare_header_string2(_temp, _flag1[2]) )
        {
            m_type = 2;            
        }
        else
        {
            _check = false;
        }
        
        DEBUG_REPORT("MIDI TYPE:" << m_type);

        if( _check )
        {
            m_file.read((char*)_temp, sizeof(_temp));            
            
            if( _temp[0] == 0 )
            {
                m_tracks = (unsigned short)_temp[1];
            }
            else
            {
                _check = false;
            }
            
        }

        if( _check )
        {
            m_file.read((char*)_temp, sizeof(_temp));            
            
            m_tickcount = _temp[0] * 256 + _temp[1];
        }
        
        
    }
        
    return _check;
}

void midiCore::endLoad()
{
    m_file.close();
    
    for( int i=0; i<MAX_MIDI_TRACK; i++ )
    {
        MIDI_TRACK*& _track = m_trackArray[i];
        if( _track )
        {
            delete _track;
            m_trackArray[i] = NULL;
        }
    }
}

void midiCore::process()
{
    DEBUG_REPORT( "track count:" << m_tracks );
    
    //body
    for( int i=0; i<m_tracks; i++ )
    {
        MIDI_BYTE _mtrkHeader[4];
        
        m_file.read( (char*)_mtrkHeader, sizeof(_mtrkHeader));
        
        const MIDI_BYTE _MTRK[] = {'M','T','r','k'};
        
        if( compare_header_string4(_mtrkHeader, _MTRK) )
        {
            ////////
            unsigned long _trackLength = 0;
            unsigned char _trackInfo[4];
            
            m_file.read((char*)&_trackInfo, sizeof(_trackInfo));
            
            _trackLength = takeDword(_trackInfo);
            
            MIDI_BUFF _BUFF(_trackLength);
            char* _buff = (char*)_BUFF.buff();
            
            m_file.read((char*)_buff, _trackLength);
            
            ////////
            m_trackArray[i] = new MIDI_TRACK;
            
            ////////            
            processTrack(&_BUFF, _trackLength, i);
            
        }
        else
        {
            DEBUG_REPORT( "ERROR TRACK(index):"<<i );
        }
        
    }    
    
}

template<class T>
T* CREATE_OBJECT()
{
    T* t = new T;
    memset(t, 0, sizeof(T));
    
    return t;
}

void midiCore::processTrack(MIDI_BUFF* _BUFF, unsigned long long _buffsize, int _trackIndex)
{
    ////////
    MIDI_BYTE* _byte = NULL;
    
    ////////
    MIDI_BYTE* _buff = _BUFF->buff();
    
    ////////
    MIDI_TRACK*& _miditrack = m_trackArray[_trackIndex];
    
    ////////
    DEBUG_REPORT( "track index:" << _trackIndex << " track length:" << _buffsize );
    
    ////////
    MIDI_BYTE     _event;
    MIDI_BYTE     _chanel;
    
    point_ctrl _point(_buff, _buffsize);
    
    MIDI_BYTE     _oldEvent = 0;
    MIDI_BYTE     _recEvent = 0;
    
    ////////    
    for(;;)
    {
        ////////
        int _readbytecount = 0;
        
        _byte = _point.point();         
        
        const MIDI_BYTE* _delayPoint = _byte; 
        const int _delay = takeReadByte(_byte, _readbytecount);        
        
#ifdef MIDI_DEBUG_REPORT
        if( _delay < 0 )
        {
            DEBUG_REPORT("***delay error***:"<<_delay);
        }
#endif
        
        if( !_point.next(_readbytecount) && _point.checkEnd(1) )
        {
            goto TRACK_END;;
        }
        
        const MIDI_BYTE _eValue = _point.value();
        
        _event  = _eValue & 0xF0;      
        _chanel = _eValue & 0x0F;
        
        if( _chanel > 15 )
        {
            DEBUG_REPORT("******exception channel:"<<(int)_chanel<<"******");
        }
        
        if( _event >= 0x00 && _event <= 0x7F )
        {
            const MIDI_BYTE* _byte = _point.point();            
            _event = _oldEvent;
        }
        else
        {
            if( !_point.next() )
            {
                DEBUG_REPORT( "MIDI EVENT UNKNOW" );
                break;
            }            
            
            _oldEvent = _event;
            _recEvent = _event;
        }
        
        _byte = _point.point();       
                        
        switch(_event)
        {
            case MIDI_ENT_CLOSENOTE:
            {
                MIDI_BYTE bNote,bVal;

                if( _point.checkEnd(2) )
                {
                    bNote = _point.value();
                    _point.next();
                    
                    bVal  = _point.value();
                    _point.next();

                    ////////
                    MIDI_EVENT_NOTE* _me = CREATE_OBJECT<MIDI_EVENT_NOTE>();
                                       
                    _me->_delay   = _delay;                    
                    _me->_channel = _chanel;
                    _me->_event   = EME_CLOSENOTE;
                    
                    _me->_bNote   = bNote;
                    _me->_bVel    = bVal;
                    
                    _miditrack->push_back(_me);
                }
                else
                {
                    DEBUG_REPORT("MIDI_ENT_CLOSENOTE");
                    goto TRACK_END;
                }
                
                break;
            }
            case MIDI_ENT_OPENNOTE:
            {
                MIDI_BYTE bNote,bVal;

                if( _point.checkEnd(2) )
                {
                    bNote = _point.value();
                    _point.next();
                    
                    bVal  = _point.value();
                    _point.next();
                    
                    ////////
                    MIDI_EVENT_NOTE* _me = CREATE_OBJECT<MIDI_EVENT_NOTE>();
                    
                    _me->_delay   = _delay;                    
                    _me->_channel = _chanel;
                    _me->_event   = EME_OPENNOTE;
                    
                    _me->_bNote   = bNote;
                    _me->_bVel    = bVal;
                    
                    _miditrack->push_back(_me);
                }
                else
                {
                    DEBUG_REPORT("MIDI_ENT_OPENNOTE");
                    goto TRACK_END;
                }
                                
                break;
            }
            case MIDI_ENT_TOUCHBOARD:
            {
                MIDI_BYTE bNote,bVal;

                if( _point.checkEnd(2) )
                {
                    bNote = _point.value();
                    _point.next();
                    
                    bVal  = _point.value();
                    _point.next();
                    

                    ////////
                    MIDI_EVENT_NOTE* _me = CREATE_OBJECT<MIDI_EVENT_NOTE>();
                    
                    _me->_delay   = _delay;                    
                    _me->_channel = _chanel;
                    _me->_event   = EME_TOUCHBOARD;
                    
                    _me->_bNote   = bNote;
                    _me->_bVel    = bVal;
                    
                    _miditrack->push_back(_me);
                }
                else
                {
                    DEBUG_REPORT("MIDI_ENT_TOUCHBOARD");
                    goto TRACK_END;
                }
                                
                break;
            }
            case MIDI_ENT_SWITCHCTRL:
            {                                
                MIDI_BYTE bReg,bVal;

                if( _point.checkEnd(2) )
                {
                    bReg = _point.value();
                    _point.next();
                    
                    bVal  = _point.value();
                    _point.next();

                    ////////
                    MIDI_EVENT_SWITCH* _me = CREATE_OBJECT<MIDI_EVENT_SWITCH>();
                    
                    _me->_delay   = _delay;                    
                    _me->_channel = _chanel;
                    _me->_event   = EME_SWITCHCTRL;
                    
                    _me->_bReg    = bReg;
                    _me->_bVal    = bVal;
                    
                    _miditrack->push_back(_me);
                }
                else
                {
                    DEBUG_REPORT("MIDI_ENT_SWITCHCTRL");
                    goto TRACK_END;
                }
                                
                break;
            }
            case MIDI_ENT_CHANGEPROG:                
            {
                MIDI_BYTE _bInstrument;
                   
                if( _point.checkEnd(1) )
                {
                    _bInstrument = _point.value();
                    _point.next();
                 
                    ////////
                    MIDI_EVENT_INSTRUMENT* _me = CREATE_OBJECT<MIDI_EVENT_INSTRUMENT>();
                    
                    _me->_delay   = _delay;                    
                    _me->_channel = _chanel;
                    _me->_event   = EME_CHANGEPROG;
                    
                    _me->_instrument = _bInstrument;
                    
                    _miditrack->push_back(_me);
                }
                else
                {
                    DEBUG_REPORT("MIDI_ENT_CHANGEPROG");
                    goto TRACK_END;
                }

                break;
            }
            case MIDI_ENT_CHANNELCTRL:
            {                                
                MIDI_BYTE bChannel;
                   
                if( _point.checkEnd(1) )
                {
                    bChannel = _point.value();
                    _point.next();

                    ////////
                    MIDI_EVENT_CHANNEL* _me = CREATE_OBJECT<MIDI_EVENT_CHANNEL>();
                    
                    _me->_delay   = _delay;                    
                    _me->_channel = _chanel;
                    _me->_event   = EME_CHANNELCTRL;
                    
                    _me->_channel = bChannel;
                    
                    _miditrack->push_back(_me);
                }
                else
                {
                    DEBUG_REPORT("MIDI_ENT_CHANNELCTRL");
                    goto TRACK_END;
                }
                                
                break;
            }
            case MIDI_ENT_GLIDE:
            {                     
                unsigned short wPitch;
                   
                if( _point.checkEnd(2) )
                {
                    MIDI_BYTE* _mbPitch = _point.point();                    
                    wPitch = takeWord(_mbPitch);
                    _point.next(2);
                                        
                    ////////
                    MIDI_EVENT_GLIDE* _me = CREATE_OBJECT<MIDI_EVENT_GLIDE>();

                    _me->_delay   = _delay;                    
                    _me->_channel = _chanel;
                    _me->_event   = EME_GLIDE;
                    
                    _me->_glide = wPitch;
                    
                    _miditrack->push_back(_me);
                    
                }
                else
                {
                    DEBUG_REPORT("MIDI_ENT_GLIDE");
                    goto TRACK_END;
                }
                
                break;
            }
            case MIDI_ENT_META:
            {
                MIDI_EVENT_META* _me = CREATE_OBJECT<MIDI_EVENT_META>();
                _miditrack->push_back(_me);
                
                _me->_delay   = _delay;                                    
                _me->_channel = _chanel;                
                _me->_event   = EME_META;
                
                _me->_meta._status = EMMS_UNKNOWN;
                
                if( _eValue == 0xFF )
                {
                    ////////
                    //元数据
                    MIDI_BYTE bType,bBytes;
                    size_t CurrentPos;
                
                    bType  = _point.value();       
                    
                    ////////
                    _me->_meta._status = (ENUM_MIDI_META_STATUS)bType;

                                        
                    if( _point.next() )
                    {
                        if( bType == 0x2F )
                        {
                            DEBUG_REPORT("MIDI TRACK END");
                            goto TRACK_END;
                        }
                    }
                    else    
                    {
                        DEBUG_REPORT("MIDI_ENT_META(unknown) type:"<<bType);
                        goto TRACK_END;
                    }
                    
                    if( _point.checkEnd(1) )
                    {                        
                        bBytes = _point.value();                        
                        _point.next();

                        if( _point.checkEnd(bBytes) )
                        {                         
                            _byte = _point.point();
                            
                            _me->_meta._status = (ENUM_MIDI_META_STATUS)bType;
                            memcpy(_me->_meta._data, _byte, bBytes);
                            
                            this->processMeta(&_point, _trackIndex, bType, bBytes);    
                        }
                        else
                        {
                            DEBUG_REPORT("MIDI_ENT_META_CELL");                        
                        }
                    }
                    else
                    {
                        DEBUG_REPORT("MIDI_ENT_META(unknown) is MAX");
                        goto TRACK_END;
                    }
                }
                else if( _eValue == 0xF0 )
                {
                    //系统码
                    MIDI_BYTE bSysCode=0;
                    
                    for(;;)
                    {
                        bSysCode = _point.value();
                        _point.next();
                        
                        if(bSysCode!=0xF7)
                        {
                            
                        }
                        else
                        {
                            DEBUG_REPORT("MIDE_META_SYS");
                            break;//读取到0xF7结束
                        }
                    }
                }
                else
                {
                    _point.next();
                    DEBUG_REPORT("MIDE_META__1(unknown)");
                }                
                                                
                break;
            }
            default:
            {
                unsigned int _tvalue = (unsigned int)_event;
                DEBUG_REPORT("MIDE__1(unknown)");
                                               
                break;
            }
        }                     
    }        
    
TRACK_END:    
    
    DEBUG_REPORT("TRACK LENGTH:" << _miditrack->_eventArray.size());
}

void stdoutMetaName( const char* _title, MIDI_BYTE* _p, unsigned long _len )
{
    char _strTemp[255];
    memcpy( _strTemp, (char*)_p, _len);
    
    std::cout << _title << ":" << _strTemp << std::endl;
}

void midiCore::processMeta(point_ctrl* _point, int _trackIndex, unsigned char _metaType, MIDI_BYTE _metaBytes)
{
    int _type = (int)_metaType;
    MIDI_BYTE* _byte = NULL;
    
#define LOG_META(x) stdoutMetaName(x, _point->point(), _type)
    
    switch(_metaType)
    {
        case MIDI_META_TRACKSEQ:
        {
            LOG_META("MIDI_META_TRACKSEQ");

            unsigned short wTrackSeq;
            MIDI_BYTE* _pValue = _point->point();
            
            wTrackSeq = takeWord(_pValue);
            
            _point->next(2);
            
            break;
        }
        case MIDI_META_MUSICINFO:
        {
            LOG_META("MUSIC INFO");
            _point->next(_metaBytes);            
            break;
        }
        case MIDI_META_COPYRIGHT:
        {
            LOG_META("COPYRIGHT");
            _point->next(_metaBytes);
            break;
        }
        case MIDI_META_MUSICTITLE:
        {    
            LOG_META("MUSIC TITLE");
            _point->next(_metaBytes);            
            break;
        }
        case MIDI_META_INST_NAME:
        {      
            LOG_META("INSTRUMENT");            
            _point->next(_metaBytes);
            break;
        }
        case MIDI_META_LYRIC:
        {         
            LOG_META("MIDI_META_LYRIC");            
            _point->next(_metaBytes);
            break;
        }
        case MIDI_META_START:
        {
            LOG_META("MIDI_META_START");    
            _point->next(_metaBytes);
            break;
        }
        case MIDI_META_TRACKSTART:
        {
            //音轨开始
            DEBUG_REPORT("TRACK BEGIN");
            _point->next();
            
            break;
        }
        case MIDI_META_TRACKEND:
        {
            //音轨结束
            DEBUG_REPORT("TRACK END");
            _point->next();
            
            break;
        }
        case MIDI_META_SPEED:
        {
            MIDI_BYTE v1,v2,v3;
            
            v1 = _point->value();
            _point->next();

            v2 = _point->value();
            _point->next();

            v3 = _point->value();
            _point->next();
            
            _byte = _point->point();
            
            DEBUG_REPORT("MIDI_META_SPEED"<<" v1:"<<(int)v1<<" v2:"<<(int)v2<<" v3:"<<(int)v3);
            
            break;
        }
        case MIDI_META_BEAT:
        {
            MIDI_BYTE bBeatNumerator,bBeatDenominator,bMetronomeTimer,bNb32ndNotesInCrotchet;
            
            bBeatNumerator = _point->value();
            _point->next();
            
            bBeatDenominator = _point->value();
            _point->next();
            
            bMetronomeTimer = _point->value();
            _point->next();
            
            bNb32ndNotesInCrotchet = _point->value();
            _point->next();
                        
            DEBUG_REPORT("MIDI META BEAT:"
                    <<(int)bBeatNumerator<<","
                    <<(int)bBeatDenominator<<","
                    <<(int)bMetronomeTimer<<","
                    <<(int)bNb32ndNotesInCrotchet);
            
            break;
        }
        case MIDI_META_TUNE:
        {
            unsigned short  _bTunePitch;
            MIDI_BYTE       _bTuneType;
            
            DEBUG_REPORT("MIDI_META_TUNE");
            
            _point->next(2);
            _point->next(1);
            
            break;
        }
        case MIDI_META_SORT:
        {
            DEBUG_REPORT("MIDI_META_SORT");
            
            _point->next(_metaBytes);    
            break;
        }
    }
}

int midiCore::getTrackCount()
{
    return m_tracks;
};

unsigned long midiCore::getTrackLength(int index)
{
    MIDI_TRACK* _track = getTrack(index);
    
    int _count = 0;
    
    if( _track )
    {
        _count = _track->_eventArray.size();
    }
    
    return _count;
};

MIDI_TRACK* midiCore::getTrack(int index)
{
    MIDI_TRACK* _track = NULL;
    
    if( index < MAX_MIDI_TRACK )
    {
        _track = m_trackArray[index];
    }
    
    return _track;
};

////////////////
MidiSystem::MidiSystem():
m_nodeAlloc(4096)            
{
    
}

MidiSystem::~MidiSystem()
{
    
}

MIDI_NODE* MidiSystem::createNode()
{
    MIDI_NODE* _node = m_nodeAlloc.create();
    memset(_node, 0, sizeof(MIDI_NODE));

	return _node;
}

void MidiSystem::pushEventToNode(MIDI_NODE*& node, MIDI_EVENT*& event)
{
    for( int i=0; i<MAX_MIDI_EVENT_IN_NODE; i++ )
    {
        if( node->_event[i] == NULL )
        {
            node->_event[i] = event;
            break;
        }
    }
}

bool MidiSystem::processTrack(MIDI_TRACK* track)
{
    if( !track )
    {
        return false;
    }
    
    bool check = false;
    m_maxEvent = 0;
    
    MIDI_EVENT_ARRAY& _eventArray = track->_eventArray;
    
    if( _eventArray.size() > 0 )
    {
        MIDI_NODE* _node = createNode();        
        m_nodeArray.push_back(_node);
        int _eventindex = 0;

        for( int i=0; i<_eventArray.size(); i++ )
        {            
            ////////
            MIDI_EVENT* evt = _eventArray[i];

            if( evt->_delay == 0 || i == 0 )
            {
                pushEventToNode(_node, evt);
                _eventindex += 1;
            }
            else
            {                
                _node = createNode();
                m_nodeArray.push_back(_node);
                _eventindex = 0;
                
                _node->_delay = evt->_delay;
                
                pushEventToNode(_node, evt);                
                _eventindex += 1;
            }
            
            if( m_maxEvent < _eventindex )
            {
                m_maxEvent = _eventindex;
            }
            
            if( evt->_event == EME_META )
            {
                MIDI_EVENT_META* _pmem = (MIDI_EVENT_META*)evt;
                
                if( _pmem->_meta._status == EMMS_TRACKEND )
                {
                    check = true;
                    break;
                }
            }
        }
        
    }
    
    if( !check )
    {
        DEBUG_REPORT("PROCESS FAILED");

        clean();
    }
    else
    {
        DEBUG_REPORT("PROCESS SUCCESS");
    }
    
    return check;
}

void MidiSystem::clean()
{
    for( int i=0; i<m_nodeArray.size(); i++ )
    {
        MIDI_NODE* _node = m_nodeArray[i];
        
        if( _node )
        {
            m_nodeAlloc.release(_node);
        }        
    }
    
    m_nodeArray.clear();
}

MIDI_NODE_ARRAY& MidiSystem::getList()
{
	return m_nodeArray;
}

////////
bool checkCloseNote(MIDI_BYTE _bnote, MIDI_NODE* _node)
{
    bool _check = false;
    
    for( int j=0; j<MAX_MIDI_EVENT_IN_NODE; j++ )
    {
        MIDI_EVENT* _event = _node->_event[j];
            
        if( _event != NULL && _event->_event == EME_CLOSENOTE )
        {
            MIDI_EVENT_NOTE* _pmeo = (MIDI_EVENT_NOTE*)_event;                         
            const MIDI_BYTE _bNote = _pmeo->_bNote;

            if( _bNote == _bnote )
            {
                _check = true;
                break;                
            }            
        }
    }
    
    return _check;
}

bool checkOpenNote(MIDI_NODE* _node)
{
        bool _check = false;
    
    for( int j=0; j<MAX_MIDI_EVENT_IN_NODE; j++ )
    {
        MIDI_EVENT* _event = _node->_event[j];
            
        if( _event != NULL && _event->_event == EME_OPENNOTE )
        {
            _check = true;
            break;          
        }
    }
    
    return _check;
}

////////
MidiNoteSystem::MidiNoteSystem(MIDI_NODE_ARRAY& _array):
m_noteAlloc(4096),
m_noteNodeAlloc(1024)        
{
    ////////    
    NOTE_ARRAY _noteArray;   
    
    ////////
    unsigned int _tickCount = 0;
    for( int i=0; i<_array.size(); i++ )
    {
        MIDI_NODE* _node = _array[i];
        
        _tickCount += _node->_delay;
        
        for( int j=0; j<MAX_MIDI_EVENT_IN_NODE; j++ )
        {
            MIDI_EVENT* _event = _node->_event[j];
            
            if( _event != NULL && _event->_event == EME_OPENNOTE )
            {
                MIDI_EVENT_NOTE* _pmeo = (MIDI_EVENT_NOTE*)_event;                
                const MIDI_BYTE _bNote = _pmeo->_bNote;
                
                //find close
                unsigned int _tickLast  = 0;
                //unsigned int _tickLastCount = 0;
                for( int k=i+1; k<_array.size() - i - 1 ;k++ )
                {
                    MIDI_NODE* _nextNode = _array[k];
                    _tickLast += _nextNode->_delay;
                    //_tickLastCount = _tickLast;
                    
                    if( 
                            checkCloseNote(_bNote, _nextNode) 
                            //checkOpenNote(_nextNode)
                      )
                    {                                
                        break;
                    }                    
                }
                
                if( _tickLast == 0 && i < _array.size() )
                {
                    MIDI_NODE* _nextNode = _array[i+1];
                    _tickLast = _nextNode->_delay;
                }
                
                ////push data
                {   
                    NOTE* _nNote = m_noteAlloc.create();
                    
                    _nNote->_bNote = _bNote;
                    _nNote->_bVel  = _pmeo->_bVel;
                    _nNote->_start = _tickCount;
                    _nNote->_end   = _tickCount + _tickLast;
                    _nNote->_last  = _tickLast;                 
                    //_nNote->_tickcount = _tickLastCount;
                    
                    _noteArray.push_back(_nNote);

                }                
            }
        }        
    }    


    DEBUG_REPORT("****** note count :"<< _noteArray.size() << "******" << std::endl);
            
    ////////       
    unsigned long _tickIndex = 0xFFFFFFFF;
    NOTE_NODE* _midiNote = NULL;
    int _midiNoteIndex = 0;
        
    for( int i=0; i< _noteArray.size(); i++ )
    {
        NOTE* _note = _noteArray[i];
         
        if( _tickIndex != _note->_start )
        {
            _tickIndex = _note->_start;
                
            _midiNote = m_noteNodeAlloc.create();
            m_noteNodeArray.push_back( _midiNote );
                
            _midiNoteIndex = 0;
        }
            
        _midiNote->_note[_midiNoteIndex++] = _note;
    } 
    
}


MidiNoteSystem::~MidiNoteSystem()
{
    
}

MidiNoteSystem::NOTE_NODE_ARRAY& MidiNoteSystem::getNoteArray()
{
    return m_noteNodeArray;
}

ENUM_MIDI_VOICE_HEIGHT MidiNoteSystem::NOTE::getHeight()
{
    int _noteValue = (int)_bNote;
    
    ENUM_MIDI_VOICE_HEIGHT _emvh = (ENUM_MIDI_VOICE_HEIGHT)(_noteValue % 12);
    return _emvh;
}

const char* MidiNoteSystem::NOTE::getHeightName()
{
    const char* _heightName[] =
    {
        "C",
        "C#",
        "D",
        "D#",
        "E",
        "F",
        "F#",
        "G",
        "G#",
        "A",
        "A#",
        "B"
    };
    
    return _heightName[getHeight()];    
}

int MidiNoteSystem::NOTE::getLevel()
{
    int _noteValue = (int)_bNote;
    
    int _result = (int)(_noteValue / 12);
    
    return _result;
}

