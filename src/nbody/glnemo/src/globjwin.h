// ============================================================================
// Copyright Jean-Charles LAMBERT - 2004-2005                                  
// e-mail:   Jean-Charles.Lambert@oamp.fr                                      
// address:  Dynamique des galaxies                                            
//           Laboratoire d'Astrophysique de Marseille                          
//           2, place Le Verrier                                               
//           13248 Marseille Cedex 4, France                                   
//           CNRS U.M.R 6110                                                   
// ============================================================================
// See the complete license in LICENSE and/or "http://www.cecill.info".        
// ============================================================================
// definition of the main Widget Class                                         
//                                                                             
// ============================================================================

#ifndef GLOBJWIN_H
#define GLOBJWIN_H

#include <qmainwindow.h>
#include <qwidget.h>
#include <qvbox.h>
#include <qframe.h>
#include <qevent.h>
#include <qlistbox.h> 
#include <pthread.h>

#include "virtual_particles_select.h"
#include "set_particles_range_form.h"
#include "snapshot_data.h"
#include "virtual_data.h"
#include "acquire_data_thread.h"

#include "global_options.h"
#include "options_form.h"

class GLBox;

class GLObjectWindow : public QMainWindow
// public QWidget
{
    Q_OBJECT
public:
  GLObjectWindow( QWidget* parent = 0, const char* name = 0 );
  ~GLObjectWindow();
   void connectToHostname( QString );      
   void connectToHostname( QListBoxItem*); 
   void takeScreenshot(QString);
   void setObjectVisible(bool, int);
private:
  // particle range vector
  ParticlesSelectVector psv; // store Particles Range
  int nbody, * i_max;
  float * pos,* coo_max, timu;
  VirtualData * virtual_data;
  bool play_animation;  
  int play_timer;

protected slots:
    void deleteFirstWidget();
    void selectFileOpen();
    void selectConnexionOpen();

    
private slots:
    void optionsToggleFullScreen();
    void optionsToggleGrid();
    void optionsToggleTranslation();
    void optionsReset();
    void optionsFitAllPartOnScreen();
    void optionsFitAllPartOnScreenPersp();
    void optionsFitAllPartOnScreenOrtho(float);
    void optionsParticlesRange();
    void optionsBlending();
    void optionsTogglePlay();
    void optionsCamera();
    void optionsLookForNetworkServer();
    void optionsReloadSnapshot();
    void messageWarning(QString *,int);
    void infoMessage(std::string );
    void optionsRecord();
    void optionsToggleGazParticles();
    void optionsPrintBuffer();
    void setProjection();
    void callMe();
public:
    GLBox  * glbox;     // OpenGL engine        
    GlobalOptions * store_options; // store global options
private:
    //GLBox  * glbox;     // OpenGL engine
    QHBox  * glframe;   // OpenGL's widget container

    // OptionsForm Box
    OptionsForm * options_form;
    
    // Mouse control variable
    int last_posx,  last_posy,  last_posz,  // save pos for rotation
        last_tposx, last_tposy, last_tposz, // save pos for translation 
        x_mouse, y_mouse, z_mouse,          // total rotation
        tx_mouse, ty_mouse, tz_mouse;       // total translation
    bool is_pressed_left_button;
    bool is_pressed_right_button;
    bool is_pressed_middle_button;
    bool is_translation;
    bool is_mouse_pressed;
    bool is_key_pressed;
    bool range_visib;
    ::string select_list;
    bool new_virtual_object; // true when a new virtual data object is instantiate
    // Mutex and conditional variable
    pthread_mutex_t mutex_timer; // used during timer event

    // methods
    void initStuff();
    void wheelEvent       ( QWheelEvent  * );
    void mousePressEvent  ( QMouseEvent  * );
    void mouseReleaseEvent( QMouseEvent  * );
    void mouseMoveEvent   ( QMouseEvent  * );
    void keyPressEvent    ( QKeyEvent    * );
    void keyReleaseEvent  ( QKeyEvent    * );
    void resizeEvent      ( QResizeEvent * );
    void timerEvent       ( QTimerEvent  * );
    void setProjection(float);
    void selectListIndex();
     
    // menus and toolsbars action
    void fileOpen();
    // loading Thread
    AcquireDataThread * ad_thread;
};


#endif // GLOBJWIN_H
