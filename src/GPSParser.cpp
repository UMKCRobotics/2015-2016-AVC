#include "GPSParser.h"

void GPSParser::parseNMEAString(string nmeastring, GPSInfo& info){
  const char * line = nmeastring.c_str();
        switch (minmea_sentence_id(line, false)) {
            case MINMEA_SENTENCE_GGA: {
                struct minmea_sentence_gga frame;
                if (minmea_parse_gga(&frame, line)) {
                  info.fixQuality = frame.fix_quality;
                  info.node.latitude = minmea_rescale(&frame.latitude,100000);
                  info.node.longitude = minmea_rescale(&frame.longitude,100000);
                  info.satsInUse = frame.satellites_tracked;
                }
                else {
                  CLOG(ERROR,"gps")<< "GGA sentenced not parsed";
                }
            } break;

            case MINMEA_SENTENCE_GST: {
                struct minmea_sentence_gst frame;
                if (minmea_parse_gst(&frame, line)) {
                  info.deviation.latitude = minmea_tofloat(&frame.latitude_error_deviation);
                  info.deviation.longitude = minmea_tofloat(&frame.longitude_error_deviation);
                }
                else {
                  CLOG(ERROR,"gps") << "GST sentence not parsed correctly";
                }
            } break;
        case MINMEA_SENTENCE_GLL:{
              minmea_sentence_gll frame;
              if(minmea_parse_gll(&frame,line)){
                info.node.latitude = minmea_tocoord(&frame.latitude);
                info.node.longitude = minmea_tocoord(&frame.longitude);
                info.lastFix = frame.time.hours * 1000 + frame.time.minutes * 100 + frame.time.seconds; //todo improve this
              }
              else{
                CLOG(ERROR,"gps") << "GLL sentence not read correctly";
              }
        }
              break;
        case MINMEA_SENTENCE_GSA:{
             minmea_sentence_gsa frame;
             if(minmea_parse_gsa(&frame,line)){
               info.pdop = minmea_tocoord(&frame.pdop);
               //i guess figure out how many sats fromthis
             }
             else{
               CLOG(ERROR,"gps") << "error reading gsa string";
             }
        }break;
        case MINMEA_SENTENCE_RMC:{
          CLOG(INFO, "gps") << "ignoring RMC sentence...";
        }break;
        case CUSTOM_SENTENCE_VTG:{
          custom_sentence_vtg frame;
          if(custom_parse_vtg(&frame,line)){
            info.heading = minmea_tocoord(&frame.true_track);
          }
          else{
            CLOG(ERROR,"gps") << "VTG sentence not parsed properly";
          }
        }break;
            case MINMEA_INVALID: {
              CLOG(ERROR,"gps") << nmeastring << "is not valid";
            } break;

            default: {
              CLOG(ERROR,"gps") <<  nmeastring <<" is not parsed";
            } break;
        }
}
