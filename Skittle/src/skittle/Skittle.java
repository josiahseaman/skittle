/*
 * Update and launcher application for the Skittle project.
 * When launched, the application will look to see if Skittle is already installed.
 * If not, it will poll the server to grab all required skittle files and install them to userfolder/AppData/Skittle
 * If Skittle is already installed, it will check the version number of all files to make sure they aren't missing or out of date.
 */
package skittle;

import java.io.*;

/**
 * Main thread of the application.
 * This will do initial installation checks on Skittle then launch the UI.
 * 
 * @author bryan
 */
public class Skittle {
    /**
     * The Main GUI Window of the application.
     */
    private static MainWindow window;
    
    /**
     * If the SkittleToo.exe file is installed
     */
    private static boolean installed;
    
    /**
     * What OS we are running on
     */
    private static String OS = null;
    
    /**
     * Main entry point of the main thread
     * 
     * @param args the command line arguments
     */
    public static void main(String args[]) {
        //Find out what OS we are running on first
        getOSName();
        
        //Check to see if Skittle is installed already
        String skittlePath = null;
        File skittleExe = null;
        
        if(isWindows()){
            skittlePath = System.getProperty("user.home") + "/AppData/Roaming/Skittle/";
            skittleExe = new File(skittlePath + "SkittleToo.exe");
        }
        else if(isMac()){
            skittlePath = "/Applications/";
            skittleExe = new File(skittlePath + "SkittleToo.app");
        }
        else{
            //Unsupported OS
            OS = "BAD";
        }
        
        installed = skittleExe.exists();
        
        String openFilePath = "";
        boolean update = false;
        if(args.length != 0){
            if(args[0].equalsIgnoreCase("update")){
                update = true;
            }
            else{
                openFilePath = args[0];
            }
            
            if(args.length > 1){
                if(args[1].equalsIgnoreCase("update")){
                    update = true;
                }
            }
        }
        
        /* Create and display the Main Window */
        window = new MainWindow(installed, openFilePath, update, OS);
        if(update){
            try{
                Thread.sleep(2000);
            }
            catch(Exception e){
                
            }
        }
        java.awt.EventQueue.invokeLater(new Runnable() {
            public void run() {
                window.setVisible(true);
            }
        });
    }
    
    public static String getOSName(){
        if(OS == null){
            OS = System.getProperty("os.name");
        }
        return OS;
    }
    
    public static boolean isWindows(){
        return OS.startsWith("Windows");
    }
    
    public static boolean isMac(){
        return OS.startsWith("Mac");
    }
}
