#ifndef MYHELPER_H
#define MYHELPER_H

#include <QFile>
#include <QTextCodec>
#include <QMessageBox>
#include <QAbstractButton>
#include <QCoreApplication>
#include <QFileDialog>
#include <QTime>
#include <QProcess>
#include <QDir>
#include <QApplication>
#include <QStyleFactory>
#include <QInputDialog>
#include <QStyle>
#include <QSettings>
#include <QSqlQuery>
#include <QVariant>
#include <QDateTime>
#include <QKeyEvent>
#include <QDebug>
#include <QNetworkInterface>
#include <QCryptographicHash>

/* 说明:全局辅助操作类实现文件
 * 功能:窗体显示/编码设置/开机启动/文件选择与复制/对话框等
 * 作者:刘典武  QQ:517216493
 * 时间:2013-12-30  检查:2014-1-10
 */
class myHelper:public QObject
{

public:

    //设置为开机启动
    static void AutoRunWithSystem(bool IsAutoRun, QString AppName, QString AppPath)
    {
        QSettings *reg=new QSettings(
                    "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
                    QSettings::NativeFormat);

        if (IsAutoRun){reg->setValue(AppName,AppPath);}
        else{reg->setValue(AppName,"");}
//        delete reg;
    }

    //设置编码为GB2312
    static void SetGB2312Code()
    {
        QTextCodec *codec=QTextCodec::codecForName("GB2312");
        QTextCodec::setCodecForLocale(codec);        
    }

    //设置编码为UTF8
    static void SetUTF8Code()
    {
        QTextCodec *codec=QTextCodec::codecForName("UTF-8");
        QTextCodec::setCodecForLocale(codec);        
    }

    //设置应用程序样式
    static void SetStyle()
    {        
        QString qss="";
        qss+="QLineEdit{border:1px solid gray;border-radius:7px;padding:3px;}";
        qss+="QListWidget{border:1px solid gray;}";
        qss+="QListWidget::item{padding:0px;margin:3px;}";
        qss+="QTreeWidget{border:1px solid gray;}";
        qss+="QTreeWidget::item{padding:0px;margin:3px;}";
        qss+="QTableWidget{border:1px solid gray;}";
        qss+="QTableView{border:1px solid gray;}";
        qss+="QScrollArea{border:1px solid gray;}";        
        qApp->setStyleSheet(qss);
    }

    //判断是否是IP地址
    static bool IsIP(QString IP)
    {
        QRegExp RegExp("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)");
        return RegExp.exactMatch(IP);
    }

    //显示信息框,仅确定按钮
    static void ShowMessageBoxInfo(QString info)
    {
        QMessageBox msg;
        //msg.setStyleSheet("font: 12pt '宋体';");
        msg.setWindowTitle("提示");
        msg.setText(info);
        msg.setIcon(QMessageBox::Information);
        msg.setCursor(QCursor(Qt::PointingHandCursor));
        msg.addButton("确定",QMessageBox::ActionRole);
        msg.setWindowFlags(Qt::FramelessWindowHint);//去掉标题栏

        msg.exec();
    }    

    //显示错误框,仅确定按钮
    static void ShowMessageBoxError(QString info)
    {
        QMessageBox msg;
        //msg.setStyleSheet("font: 12pt '宋体';");
        msg.setWindowTitle("错误");
        msg.setText(info);
        msg.setIcon(QMessageBox::Critical);
        msg.setCursor(QCursor(Qt::PointingHandCursor));
        msg.addButton("确定",QMessageBox::ActionRole);
        msg.setWindowFlags(Qt::FramelessWindowHint);//去掉标题栏

        msg.exec();
    }    

    //显示询问框,确定和取消按钮
    static int ShowMessageBoxQuesion(QString info)
    {
        QMessageBox msg;
        //msg.setStyleSheet("font: 12pt '宋体';");
        msg.setWindowTitle("询问");
        msg.setText(info);
        msg.setIcon(QMessageBox::Question);
        msg.setCursor(QCursor(Qt::PointingHandCursor));
        msg.addButton("确定",QMessageBox::ActionRole);
        msg.addButton("取消",QMessageBox::RejectRole);
        msg.setWindowFlags(Qt::FramelessWindowHint);//去掉标题栏

        return msg.exec();
    }    

    //显示标准输入框
    static QString ShowInputBox(QWidget *frm,QString info)
    {
        bool ok;
        return QInputDialog::getText(frm,"提示",info,QLineEdit::Password,"",&ok);
    }

    //16进制字符串转字节数组
    static QByteArray HexStrToByteArray(QString str)
    {
        QByteArray senddata;
        int hexdata,lowhexdata;
        int hexdatalen = 0;
        int len = str.length();
        senddata.resize(len/2);
        char lstr,hstr;
        for(int i=0; i<len; ) {
            hstr=str[i].toLatin1();
            if(hstr == ' ') {
                i++;
                continue;
            }
            i++;
            if(i >= len)
                break;
            lstr = str[i].toLatin1();
            hexdata = ConvertHexChar(hstr);
            lowhexdata = ConvertHexChar(lstr);
            if((hexdata == 16) || (lowhexdata == 16))
                break;
            else
                hexdata = hexdata*16+lowhexdata;
            i++;
            senddata[hexdatalen] = (char)hexdata;
            hexdatalen++;
        }
        senddata.resize(hexdatalen);
        return senddata;
    }

    static char ConvertHexChar(char ch)
    {
        if((ch >= '0') && (ch <= '9'))
            return ch-0x30;
        else if((ch >= 'A') && (ch <= 'F'))
            return ch-'A'+10;
        else if((ch >= 'a') && (ch <= 'f'))
            return ch-'a'+10;
        else return (-1);
    }
    static int HexValue(unsigned char a)
    {
        a -= 48;
        if(a >32) a -=32;// 'a' 0x61
        if(a > 9 ) a -= 7;// 'A' 0x41
        return a;
    }
    //
    static int HexStrValue(unsigned char stra,unsigned char strb )
    {
        int v=0;
        v=HexValue(stra)<<4;
        v=v+HexValue(strb);
        return v;
    }

    //字节数组转16进制字符串
    static QString ByteArrayToHexStr(QByteArray data)
    {
        QString temp="";
        QString hex=data.toHex();
        for (int i=0;i<hex.length();i=i+2){
            temp+=hex.mid(i,2)+" ";
        }
        return temp.trimmed().toUpper();
    }

    //16进制字符串转10进制
    static int StrHexToDecimal(QString strHex)
    {
        bool ok;
        return strHex.toInt(&ok,16);
    }

    //10进制字符串转10进制
    static int StrDecimalToDecimal(QString strDecimal)
    {
        bool ok;
        return strDecimal.toInt(&ok,10);
    }

    //2进制字符串转10进制
    static int StrBinToDecimal(QString strBin)
    {
        bool ok;
        return strBin.toInt(&ok,2);
    }

    //16进制字符串转2进制字符串
    static QString StrHexToStrBin(QString strHex)
    {
        uchar decimal=StrHexToDecimal(strHex);
        QString bin=QString::number(decimal,2);
        uchar len=bin.length();
        if (len<8){
            for (int i=0;i<8-len;i++){
                bin="0"+bin;
            }
        }
        return bin;
    }

    //10进制转2进制字符串一个字节
    static QString DecimalToStrBin1(int decimal)
    {
        QString bin=QString::number(decimal,2);
        uchar len=bin.length();
        if (len<=8){
            for (int i=0;i<8-len;i++){
                bin="0"+bin;
            }
        }
        return bin;
    }

    //10进制转2进制字符串两个字节
    static QString DecimalToStrBin2(int decimal)
    {
        QString bin=QString::number(decimal,2);
        uchar len=bin.length();
        if (len<=16){
            for (int i=0;i<16-len;i++){
                bin="0"+bin;
            }
        }
        return bin;
    }

    //10进制转16进制字符串,补零.
    static QString DecimalToStrHex(int decimal)
    {
        QString temp=QString::number(decimal,16);
        if (temp.length()==1){
            temp="0"+temp;
        }
        return temp;
    }

    //延时
    static void Sleep(int sec)
    {
        QTime dieTime = QTime::currentTime().addMSecs(sec);
        while( QTime::currentTime() < dieTime )
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }    

    //设置系统日期时间
    static void SetSystemDateTime(int year,int month,int day,int hour,int min,int sec)
    {
        QProcess p(0);

        p.start("cmd");
        p.waitForStarted();
        p.write(QString("date %1-%2-%3\n").arg(year).arg(month).arg(day).toLatin1());
        p.closeWriteChannel();
        p.waitForFinished(1000);
        p.close();

        p.start("cmd");
        p.waitForStarted();
        p.write(QString("time %1:%2:%3.00\n").arg(hour).arg(min).arg(sec).toLatin1());
        p.closeWriteChannel();
        p.waitForFinished(1000);
        p.close();
    }

    //窗体居中,并且只有关闭按钮,不能调整大小
    static void FormOnlyCloseInCenter(QWidget *frm,int deskWidth,int deskHeigth)
    {        
        int frmX=frm->width();
        int frmY=frm->height();
        QPoint movePoint(deskWidth/2-frmX/2,deskHeigth/2-frmY/2);
        frm->move(movePoint);                          //窗体居中
        frm->setFixedSize(frmX,frmY);                  //窗体不能调整大小
        frm->setWindowFlags(frm->windowFlags() & Qt::WindowCloseButtonHint);//窗体只有关闭按钮
//        frm->setAttribute(Qt::WA_DeleteOnClose);       //关闭时自动释放资源
    }

    //窗体居中显示
    static void FormInCenter(QWidget *frm,int deskWidth,int deskHeigth)
    {        
        int frmX=frm->width();
        int frmY=frm->height();
        QPoint movePoint(deskWidth/2-frmX/2,deskHeigth/2-frmY/2);
        frm->move(movePoint);
    }    

    //窗体不能改变大小
    static void FormNotResize(QWidget *frm)
    {
        frm->setFixedSize(frm->width(),frm->height());
    }    

    //获取选择的文件
    static QString GetFileName(QString filter)
    {
        return QFileDialog::getOpenFileName(0,"选择文件",QCoreApplication::applicationDirPath(),filter);
    }

    //获取选择的文件集合
    static QStringList GetFileNames(QString filter)
    {
        return QFileDialog::getOpenFileNames(0,"选择文件",QCoreApplication::applicationDirPath(),filter);
    }

    //获取选择的目录
    static QString GetFolderName()
    {
        return QFileDialog::getExistingDirectory();;
    }

    //获取文件名,含拓展名
    static QString GetFileNameWithExtension(QString strFilePath)
    {
        QFileInfo fileInfo(strFilePath);
        return fileInfo.fileName();
    }

    //获取选择文件夹中的文件
    static QStringList GetFolderFileNames(QStringList filter)
    {
        QStringList fileList;
        QString strFolder = QFileDialog::getExistingDirectory();
        if (!strFolder.length()==0){
            QDir myFolder(strFolder);
            if (myFolder.exists()){
                fileList= myFolder.entryList(filter);
            }
        }
        return fileList;
    }

    //文件夹是否存在
    static bool FolderIsExist(QString strFolder)
    {
        QDir tempFolder(strFolder);
        return tempFolder.exists();
    }

    //文件是否存在
    static bool FileIsExist(QString strFile)
    {
        QFile tempFile(strFile);
        return tempFile.exists();
    }

    //复制文件
    static bool CopyFile(QString sourceFile, QString targetFile)
    {
        if (FileIsExist(targetFile)){
            int ret=QMessageBox::information(0,"提示","文件已经存在,是否替换?",QMessageBox::Yes | QMessageBox::No);
            if (ret!=QMessageBox::Yes){
                return false;
            }
        }
        return QFile::copy(sourceFile,targetFile);
    }

    //异或加密算法
    static QString getXorEncryptDecrypt(QString str, char key)
    {
        QByteArray data=str.toLatin1();
        int size=data.size();
        for(int i=0; i<size; i++){
            data[i] = data[i] ^ key;
        }
        return QLatin1String(data);
    }

    //异或校验
    //从第index位开始进行校验
    static char XORValid(char *buffer,int len)
    {
        unsigned char checksum = 0;
//        char ch1,c1; //校验位的高四位和第四位
        for (int i = 0;  i<len;  i++){
        checksum ^=  buffer[i]; //进行异或交验取值
        }
        return checksum;
//        ch1 = (checksum  >> 4) & 0x0F;  //取高位数；
//        c1 = checksum & 0x0F;  //取低位数；
//        if (ch1 < 10)                       //低于10的数
//            ch1 = ch1  +  '0';
//        else
//            ch1 = (ch1 - 10 ) +  'A';   //不低于10的16进制数，如：A、B、C、D、E、F
//        if (c1 < 10) c1 = c1  +  '0';
//        else c1 = (c1 - 10 )+  'A';
//         XOR_High=ch1;
//         XOR_Low =c1;
     }
    //把一个字符串转化成浮点数
    static double Str_To_Double(char *buf)
    {
        double rev = 0;
        double dat=0;
        int integer = 1;
        char *str = buf;
        int i=0;
        while(*str != '\0')
        {
            switch(*str)
            {
            case '0':
                dat = 0;
                break;
            case '1':
                dat = 1;
                break;
            case '2':
                dat = 2;
                break;
            case '3':
                dat = 3;
                break;
            case '4':
                dat = 4;
                break;
            case '5':
                dat = 5;
                break;
            case '6':
                dat = 6;
                break;
            case '7':
                dat = 7;
                break;
            case '8':
                dat = 8;
                break;
            case '9':
                dat = 9;
                break;
            case '.':
                dat = '.';
                break;
            }
            if(dat == '.')
            {
                integer = 0;
                i = 1;
                str ++;
                continue;
            }
            if( integer == 1 )
            {
                rev = rev * 10 + dat;
            }
            else
            {
                rev = rev + dat / (10 * i);
                i = i * 10 ;
            }
            str ++;
        }
        return rev;
    }

    //GB212 CRC校验
    static int CRC16_GB212(char *databuff, int len)
    {
        int  wkg = 0xFFFF;

        while( len-- )
        {
           wkg  = (wkg>>8)&0x00FF;
           wkg ^= *databuff++;

           for (char i = 0 ; i < 8; i++ )
           {
              if ( wkg & 0x0001 )
              {
                wkg = ( wkg >> 1 ) ^ 0xa001;
              }
              else
              {
                wkg = wkg >> 1;
              }
           }
        }
        return( wkg );
    }

    static int CRC16_Modbus(char *databuff, int len)
    {
        const int auchCRCHi[] =
        {
            0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
            0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
            0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
            0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
            0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
            0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
            0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
            0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
            0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
            0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
            0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
            0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
            0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
            0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
            0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
            0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
            0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
            0x40
        } ;

        const int auchCRCLo[] =
        {
            0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
            0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
            0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
            0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
            0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
            0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
            0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
            0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
            0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
            0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
            0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
            0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
            0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
            0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
            0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
            0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
            0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
            0x40
        };
        int uchCRCHi = 0xFF ;
        int uchCRCLo = 0xFF ;
        int uIndex ;
        while (len--)
        {
            uIndex = uchCRCLo ^ *databuff++ ;
            uchCRCLo = uchCRCHi ^ auchCRCHi[uIndex] ;
            uchCRCHi = auchCRCLo[uIndex] ;
        }
        return (uchCRCHi << 8 | uchCRCLo) ;
    }

    //和计算
    static int SUM(char *databuff,int len)
    {
        int res=0;
        for(int i=0;i<len;i++)
        {
            res+=*databuff++;
        }
        return res;
    }


    //加密认证
    static bool MD5_EncryptDecrypt(QByteArray key)
    {
        QNetworkInterface  localInterface =localInterface.interfaceFromName("eth0");
        QString MAC =localInterface.hardwareAddress();
//        qDebug()<<MAC;
        QByteArray table="tinzscylinux";
        QByteArray data=MAC.toLatin1();
        int size=data.size();
        for(int i=0; i<size; i++){
            data[i] = data[i] ^ table[i%12];
        }

        QByteArray md5;
        md5 = QCryptographicHash::hash (data, QCryptographicHash::Md5 ).toHex().toUpper();
//        qDebug()<<md5;
        return ((md5==key)?true:false);
    }

    /***********************************************************************************
    * 函数名称：to_BCD;
    *
    * 函数功能描述：十进制数据转BCD;
    *
    * 输入参数：unsigned char类型natural_banary,指定需要转换的数据;
    *
    * 返回数据：unsigned char类型，返回转换后的数据;
    *
    * 注意：    none;
    ************************************************************************************/
    static u_int8_t to_BCD(u_int8_t natural_binary)
    {
      return ( ((natural_binary/10) << 4) + natural_binary%10 );
    }
    /***********************************************************************************
    * 函数名称：to_natural_binary;
    *
    * 功能描述：BCD转十进制数据;
    *
    * 输入参数：unsigned char类型BCD,指定需要转换的数据;
    *
    * 返回数据：unsigned char类型,返回转换后的数据;
    *
    * 注意：    none;
    ************************************************************************************/
    static u_int8_t to_natural_binary(u_int8_t BCD)
    {
      return(  ((BCD >> 4) * 10) + (BCD & 0x0f)  );
    }

};

#endif // MYHELPER_H
