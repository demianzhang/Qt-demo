#include "global.h"
cv::VideoCapture cap;
QMutex stoppedMutex;
QMutex stoppedMutex1;
volatile bool stopped = true;
volatile bool stopped1 = true;
int poseNum=0;
float posePoint[11][12][2] =
{{{-0.03201295012613544, 0.999487454160492},
{-0.2597615174024944, 0.9656727986625457},
{0.9938290181911429, -0.11092286780113883},
{0.998549842344521, -0.053835047633787075},
{0.22300349529716537, 0.9748176450420084},
{0.030368263674035327, 0.9995387779178075},
{-0.1617002630418683, 0.9868399185947995},
{-0.2746841460039153, 0.9615345131268559},
{-0.04314620509409085, 0.9990687688973059},
{0.22291132002970895, 0.9748387268685076},
{0.1820198470095704, 0.9832948567416656},
{0.13554893467053686, 0.99077065272932}}

,{{-0.8389670863519263, -0.5441821643697627},
{-0.05454133644568165, 0.998511513513349},
{0.9999968254595367, -0.0025197362657313335},
{0.9940541993109477, -0.10888640333976778},
{0.13069656711579977, 0.9914224162001509},
{-0.6298543751345033, -0.7767132457502731},
{0.016813813104723005, 0.9998586378528114},
{-0.9220255183848886, 0.38712910436581416},
{0.16866487543566408, 0.9856734549506098},
{0.28609984699382096, 0.9581998108693782},
{0.1540981868964056, 0.9880555393272387},
{0.37165628723804833, 0.9283704024560613}}

, {{0.9999978727962551, -0.0020626204121484675},
{0.056940031328294054, 0.998377600325815},
{0.976613883774057, -0.21500074888184179},
{0.9983763222263574, -0.05696243690163285},
{0.4169611530100347, 0.9089243075639152},
{-0.9446294061403927, -0.32813912454148053},
{-0.190384794046448, 0.9817095447207853},
{-0.3914685838630717, 0.9201914734707345},
{0.01981227608252734, 0.9998037175948236},
{0.1534196874984602, 0.9881611202065557},
{0.3784202715597327, 0.9256338898682666},
{0.19573299628272398, 0.9806572256227898}}

, {{-0.9953185457907089, 0.09664880964123884},
{-0.7964084989072004, 0.6047590452968685},
{0.9885659679917387, -0.1507890146149797},
{0.9253154854967854, -0.37919817022217867},
{-0.6597311090385393, 0.7515017390311076},
{-0.9876659323974338, 0.15657587930938718},
{-0.04305851038661659, 0.9990725522619894},
{-0.7067344390726668, 0.7074789273389301},
{0.0005730316017338505, 0.9999998358173782},
{0.2913982827893932, 0.9566018193519145},
{0.4031717541780877, 0.9151243285111392},
{0.23649112763310878, 0.9716336483216401}}

, {{-0.7516835658587142, -0.6595239319523803},
{-0.8423625626679712, 0.5389112292534349},
{0.9985808715156126, -0.05325638969287508},
{0.9999983295901789, -0.0018277901553307163},
{0.20942004481241144, 0.9778257742720671},
{-0.6365395430847397, 0.7712440664857467},
{0.015611043099389295, 0.9998781402417742},
{-0.8319125864241672, 0.5549067025627393},
{-0.01791760566146874, 0.9998394668182288},
{0.37672975102326056, 0.9263232128657644},
{0.42853368099236444, 0.9035258071882256},
{0.3312595400080047, 0.9435396743930193}}

, {{-0.7072322455619652, 0.7069812945456054},
{-0.8693195142705346, 0.49425052565317706},
{0.9931558794950778, -0.11679639987755866},
{0.9731976091603718, -0.2299704622870864},
{0.5555459969598774, 0.8314858058090084},
{-0.8801832968248917, 0.4746339262952709},
{-0.1990595621431682, 0.9799873931430803},
{-0.4046232268655575, 0.9144834849689215},
{-0.19913285548806847, 0.979972502606664},
{0.17623237406183012, 0.9843485918781675},
{0.79030213886121, 0.6127173323086239},
{0.11240552215447958, 0.9936624168142714}}

, {{0.20245991425629434, -0.9792905509190487},
{0.6797945342913664, 0.7334026119039829},
{-0.9999175282428278, -0.012842768889683913},
{-0.7163060994715371, -0.6977861935147989},
{0.19119638241776396, 0.9815518037018526},
{0.9999996317326756, -0.0008582158896597756},
{-0.10128397698406165, 0.9948575556361282},
{0.4606874670343126, 0.8875624246877001},
{0.27172322000959326, 0.9623754421781647},
{-0.06014440248789794, 0.9981896868077599},
{0.2835809569226564, 0.9589482993732407},
{-0.5244118553505652, 0.851464741470707}}

,{{0.37978397837203876, 0.9250752022251525},
{-0.36491620251100093, 0.931040367086707},
{0.9863068661258468, -0.1649204833639868},
{0.8965217987369618, -0.44299962120688374},
{0.5355573449271683, 0.8444988634063174},
{0.9617484131967022, -0.27393427991696373},
{-0.11594719837536518, 0.9932553786357785},
{0.22375436448068398, 0.9746455685919088},
{-0.26301822634262234, 0.9647908647015586},
{0.00026972103212497564, 0.9999999636252818},
{0.48038988833423213, 0.8770550468392643},
{-0.8707769100119642, 0.4916783226765399}}

, {{-0.9935471333018933, 0.11341998901247555},
{0.27372801167424626, 0.9618071405561843},
{-0.9999962912546602, -0.0027235045299624262},
{-0.9985109968234634, -0.054550794885255986},
{-0.7179818589286306, 0.6960618149628579},
{-0.9702430746578989, -0.2421329718943431},
{0.22717105805848675, 0.9738548713132709},
{-0.11221679864669093, 0.9936837475280996},
{0.3455829609723854, 0.9383882016977615},
{-0.04216376119861382, 0.9991107132052914},
{-0.17403570259740547, 0.9847393432890896},
{0.19767105193063128, 0.9802684097881751}}

,{{0.18574302439773405, -0.9825983558339505},
{-0.4058559270872752, 0.913937069194662},
{0.9999977946732647, -0.0021001544246263934},
{0.9963448955874245, -0.08542159584604106},
{0.16141270012735384, 0.9868869946643319},
{-0.8135440492304836, 0.5815032931649384},
{-0.20764481728276193, 0.9782042884058567},
{-0.7361689026803031, -0.6767978625309616},
{0.00019531913075576755, 0.9999999809252184},
{0.12835758113545154, 0.9917279522959187},
{-0.11639579573594282, 0.993202909145456},
{-0.11369050582966814, 0.9935162147062293}}

,{{0.19149678698546538, 0.9814932402081246},
{-0.13502603599894272, 0.9908420507842872},
{0.9992143634218247, -0.03963150175703432},
{0.9999994525448026, -0.001046379517650888},
{0.09928064766121379, 0.99505947209198},
{-0.29568775881207054, 0.9552846430717363},
{-0.20009823125575588, 0.9797758406126976},
{0.03508097913915948, 0.9993844730145841},
{0.09067220167797556, 0.9958807919840951},
{0.15341504382224116, 0.9881618411621752},
{0.035279374057435225, 0.9993774891231618},
{0.0010096227896574514, 0.9999994903307815}}};