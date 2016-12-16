#include <fstream>
#include <cmath>
#include <gtk/gtk.h>
#include <cstring>
#include <cstdlib>
using namespace std;

static int a[9][9];
static GtkWidget *wid[9][9],*janela;

void open_dialog(const char* c){
    GtkWidget *dialog, *label;
    dialog=gtk_dialog_new_with_buttons("Erro",GTK_WINDOW(janela),GTK_DIALOG_MODAL,GTK_STOCK_OK,GTK_RESPONSE_OK,NULL);
    label=gtk_label_new(c);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),label,0,0,0);
    gtk_widget_show_all(dialog);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

bool row(int i, int j, int n){
    for(int g=0;g<9;++g)
        if(a[i][g]==n)
            return 0;
    return 1;
}

bool column(int i, int j, int n){
    for(int g=0;g<9;++g)
        if(a[g][j]==n)
            return 0;
    return 1;
}

bool square(int i, int j, int n){
    int r=ceil((i+1)/3.);
    int c=ceil((j+1)/3.);
    for(int g=(r-1)*3;g<((r-1)*3+3);++g)
        for(int h=(c-1)*3;h<((c-1)*3+3);++h)
            if(a[g][h]==n)
                return 0;
    return 1;
}

void solve(){
    int un=0, tmp;
    bool b=1;

    for(int i=0;i<9;++i)
        for(int j=0;j<9;++j)
            if(a[i][j]==0)
                ++un;

    while(un && b){
        b=0;
        for(int i=0;i<9;++i)
            for(int j=0;j<9;++j){
                if(a[i][j])
                    continue;
                tmp=0;
                for(int x=1;x<10;++x){
                    if(row(i,j,x) && column(i,j,x) && square(i,j,x)){
                        if(!tmp)
                            tmp=x;
                        else{
                            tmp=0;
                            break;
                        }
                    }
                }
                if(tmp){
                    a[i][j]=tmp;
                    b=1;
                    --un;
                }
            }
    }
    if(!b)
        open_dialog("O sudoku não foi resolvido, tente solucionar novamente!");
    else
        
    for(int i=0;i<9;++i){
        for(int j=0;j<9;++j){
            char c[2];
            sprintf(c,"%d",a[i][j]);
            gtk_entry_set_text(GTK_ENTRY(wid[i][j]),c);
        }
    }
}

static void get_element(GtkWidget* widget, gpointer data){
    for(int i=0;i<9;++i)
        for(int j=0;j<9;++j)
            a[i][j]=atoi(gtk_entry_get_text(GTK_ENTRY(wid[i][j])));
    solve();
}

static void menu_event(GtkWidget* widget, gpointer data){
    if(strcmp(gtk_menu_item_get_label(GTK_MENU_ITEM(widget)),"Novo jogo")==0){
        for(int i=0;i<9;++i)
            for(int j=0;j<9;++j){
                a[i][j]=0;
                gtk_entry_set_text(GTK_ENTRY(wid[i][j]),"0");
            }
    }else if(strcmp(gtk_menu_item_get_label(GTK_MENU_ITEM(widget)),"Abrir jogo")==0){
        GtkWidget *dialog;
        dialog=gtk_file_chooser_dialog_new("Selecione o arquivo",GTK_WINDOW(janela),GTK_FILE_CHOOSER_ACTION_OPEN,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_OK,GTK_RESPONSE_OK,NULL);
        gtk_widget_show_all(dialog);
        gint response=gtk_dialog_run(GTK_DIALOG(dialog));
        if(response==GTK_RESPONSE_OK){
            ifstream in(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog)));
            if(in.is_open()){
                for(int i=0;i<9;++i)
                    for(int j=0;j<9;++j){
                        if(in.eof())
                            return;
                        in >> a[i][j];
                        char c[2];
                        sprintf(c,"%d",a[i][j]);
                        gtk_entry_set_text(GTK_ENTRY(wid[i][j]),c);
                    }
                    in.close();
                    gtk_widget_destroy(dialog);
                    solve();
            }
        }else{
            gtk_widget_destroy(dialog);
        }
    }else if(strcmp(gtk_menu_item_get_label(GTK_MENU_ITEM(widget)),"Exit")==0){
        gtk_main_quit();
    }
}

int main(int argc, char* argv[]){
    const char* file[] = {"Novo jogo","Abrir jogo","Exit"};

    for(int i=0;i<9;++i)
        for(int j=0;j<9;++j)
            a[i][j]=0;

    gtk_init(&argc,&argv);
    GtkWidget *vbox,*hbox,*separator,*button,*file_menu,*menu_bar,*menu_item;
    janela=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    vbox=gtk_vbox_new(0,0);
    file_menu=gtk_menu_new();
    menu_bar=gtk_menu_bar_new();
    menu_item=gtk_menu_item_new_with_label("File");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item),file_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar),menu_item);
    for(int i=0;i<3;++i){
        menu_item=gtk_menu_item_new_with_label(file[i]);
        gtk_menu_shell_append(GTK_MENU_SHELL(file_menu),menu_item);
        g_signal_connect(menu_item,"activate",G_CALLBACK(menu_event),NULL);
    }
    gtk_box_pack_start(GTK_BOX(vbox),menu_bar,0,0,0);
    for(int i=0;i<9;++i){
        hbox=gtk_hbox_new(0,0);
        for(int j=0;j<9;++j){
            wid[i][j]=gtk_entry_new();
            gtk_entry_set_max_length(GTK_ENTRY(wid[i][j]),1);
            gtk_widget_set_size_request(wid[i][j],20,20);
            gtk_box_pack_start(GTK_BOX(hbox),wid[i][j],0,0,0);
            if((j+1)%3==0){
                separator=gtk_vseparator_new();
                gtk_box_pack_start(GTK_BOX(hbox),separator,0,0,0);
            }
        }
        gtk_box_pack_start(GTK_BOX(vbox),hbox,0,0,0);
        if((i+1)%3==0){
            separator=gtk_hseparator_new();
            gtk_box_pack_start(GTK_BOX(vbox),separator,0,0,0);
        }
    }
    button=gtk_button_new_with_label("Checar sudoku");
    g_signal_connect(button,"clicked",G_CALLBACK(get_element),NULL);
    gtk_box_pack_start(GTK_BOX(vbox),button,0,0,0);
    g_signal_connect(janela,"delete-event",G_CALLBACK(gtk_main_quit),NULL);
    gtk_container_add(GTK_CONTAINER(janela),vbox);
    gtk_window_set_title(GTK_WINDOW(janela),"Sudoku");
    gtk_widget_show_all(janela);
    gtk_main();
    return 0;
}
