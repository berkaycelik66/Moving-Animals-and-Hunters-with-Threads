[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-24ddc0f5d75046c5622901739e7c5dd533143b0c8e959d652212380cedb1ea36.svg)](https://classroom.github.com/a/40vXwnsI)
[![Open in Visual Studio Code](https://classroom.github.com/assets/open-in-vscode-718a45dd9cf7e7f842a935f5ebbe5719a5e09af4491e668f4dbf3b35d5cca122.svg)](https://classroom.github.com/online_ide?assignment_repo_id=11106985&assignment_repo_type=AssignmentRepo)
Moving Animals and Hunters with Threads

## Learning Objectives
* Concurrency, 
* Posix threads

## Details (tr: açıklamalar)
### Grid of sites (tr: site lardan oluşan grid)
In this project, your task is to simulate migrating animals and hunters in 2D grid-sites:
Each site in this grid is good for a different living activity for the animals (randomly assigned):
(**tr:bu projede bir grid üzerinde hareket eden avcı ve hayvanları threadlerle simüle etmeniz istenmektedir. 
Grid aşağıdaki şekilde hayvanlar için farklı aktivitelere uygun üç cinste bir ızgaradır.**)
<table>
  <tr><td>WINTERING </td> <td>FEEDING</td><td>NESTING</td></tr>
  <tr><td>FEEDING</td><td>WINTERING</td><td>WINTERING</td></tr>
  <tr><td>NESTING</td><td>NESTING</td><td>FEEDING</td></tr>
</table>

That means grid[0][0] is good for wintering. 
The initilization of the grid  and type of sites are determined randomly by the following function:
(**tr:gridi random bir şekilde başlatan, yazdıran ve sonrasında silen fonksiyonlar verilmiştir. Msela başlatan fonksiyon**)
```
Grid initgrid(int xlength, int ylength)
```
### Animals
In the simulation, we have the following three animals (**tr:simülasyonda aşağıdaki üç tür hayvan vardır.**)
* a bird, 
* a bear, 
* a panda 

Every  simulation time unit, if any of these animals are (**tr:her bir simülasyon zamanında( threadin bekleyip tekrar devam etmesinde, döngü olarak düşünebilirsiniz, 
farklı tipteki sitelarda aşağıdaki şekilde davranmaktadırlar:**)
* at the NESTING site, then it populates 1 of its kind at that site and moves to a neighboring random location. 
    * (**tr:NESTİNG siteda kendi cinsinden bir tane daha hayvan oluşturmaktadır. Yani yeni bir thread oluşturmanız gerekiyor. Ve random komşu lokasyona gitmektedir.**)
* at the WINTERING site, 
    * All die with 0.5 probability or live and move to a neighboring random location.
      * (**tr:WINTERING siteda tüm hayvanlar 0.5 ihtimalle ölmektedirler. Yada random komşu lokasyona gitmektedirler.**)
* at the FEEDING site, 
    * All stay there with 0.8 probability or move to a neighboring random location.
      *  (**tr:FEEDING siteda tüm hayvanlar 0.8 ihtimalle kalmakta yada random komşu lokasyona gitmektedirler.**)


### Hunter
In the simulation, there are also randomly moving hunters, if they move to a site, then they kill all the animals in that site. The number of kills adds to their points. 
(**tr: simülasyonda rastgele hareket eden yine avcılar bulunmaktadır. 
Eğer avcılar bir site'a giderse bu bölgedeki tüm hayvanları öldürmektdirler. 
Öldürülen hayvan sayısı kendi pointleri olarak toplanmaktadır.**)
```
typedef struct{
   int points;
   Location location;
} Hunter;
```

The number of hunters are given at the beginning of the program as a command line argument:
 (**tr:Hunter(avcı) sayısı programa argüman olarak verilmektedir. Programınızın testler geçmesi için bu şekilde implement etmeniz önem arz etmektedir.**)
``./main 2`` creates 2 hunters (**tr:ifadesi iki hunter oluşturmaktadır.**).


### (This is given) printgrid (tr: print verildi bir şey yapmanız gerekmiyor)

This prints the grid with live animal counts and hunter as follows:
| Bear: 1, Bird: 2,  Panda: 0, Hunters: 1| Bear: 0, Bird: 2,  Panda: 10, Hunters: 2|  

### Simulation (main thread)
Simulation creates 1 thread for each animal, 1 thread for each hunter. **Each thread makes a move then sleeps for 1 millisecond**. 
The simulation ends after 1 second.
 (**tr: main threadde her bir hayvan ve her bir hunter için birer thread oluşturmanız ve bunları rastgele pozisyonlardan başlatmanız gerekiyor.
    Bu threadler 1milisaniye uyumakta ve sonra rastgele komşu pozisyona yukarıda açıklandığı şekilde hareket etmektedirler.**)
    
## What you need to do (tr: yapılacaklar)
Basically what you need to do is:
  - complete the main function, so that it creates 1 thread for each animal, 1 thread for each hunter
    - (**tr: main fonksiyonunu tamamlayarak her bir hunter ve animal için birer thread oluşturmasını sağlayınız.**)
  - complete the function ``void *simulateanimal(void *args);`` that is run by any of the animal threads
    -  (**tr: ``void *simulateanimal(void *args);`` fonksiyonunu tamamlayarak bir hayvanın hareket etmesini yukarıda açıklandığı şekilde simüle etmesini sağlayınız.**)
  - complete the function ``void *simulatehunter(void *args);`` that is run by any of the hunter threads.
    - (**tr: ``void *simulatehunter(void *args);`` fonksiyonunu tamamlayarak bir avcının hareket etmesini yukarıda açıklandığı şekilde simüle etmesini sağlayınız.**)

### Dying of an animal, (tr bir hayvanın ölmesi)

if an animal status is made DEAD by itself or by hunters, then its associated thread should free all its resources and terminate its running. 

Note: You can do this by letting the thread check its status before running/moving.
**Tr: bir hayvan ya kendisi veya hunter tarafından öldürülebilmektedir. 
Hunter tarafından öldürülmede her uykudan döndükten sonra durumunu kontrol ederek eğer ölü ise 
tüm kaynakları serbest bırakarak çalışmasını sonlandırmasını sağlayabilirsiniz**
