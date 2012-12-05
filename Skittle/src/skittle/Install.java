package skittle;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.FileInputStream;
import java.io.FileReader;
import java.io.InputStreamReader;
import java.io.LineNumberReader;
import java.net.URL;
import java.net.URLConnection;
import java.nio.charset.MalformedInputException;
import javax.swing.JOptionPane;

/**
 * The Installer for Skittle.
 * 
 * It first checks to see if the install directory exists.
 * If not, the directory is then created.
 * 
 * Once the directory is in place, we will poll the FTP server
 * and grab all the files needed for Skittle and place them in the folder.
 *
 * @author bryan
 */
public class Install implements Runnable {
    /**
     * The path to the user install area for Skittle
     */
    private File skittlePath;
    
    /**
     * The parent MainWindow that started the install process
     */
    private MainWindow window;
    
    /**
     * What OS we are running on
     */
    private static String OS = null;
    
    /**
     * Constructor for the installer object
     * 
     * @param window The parent MainWindow that started the install process
     */
    public Install(MainWindow window, String OS){
        this.OS = OS;
        
        //Set the path to the skittle install folder
        String skittlePathString = null;
        
        if(isWindows()){
            skittlePathString = System.getProperty("user.home") + "/AppData/Roaming/Skittle/";
        }
        else if(isMac()){
            skittlePathString = "/Applications/";
        }
        skittlePath = new File(skittlePathString);
        
        this.window = window;
    }
    
    /**
     * Run the installation procedure
     */
    @Override
    public void run(){
        //Check if the install directory doesn't exist
        if(!skittlePath.exists()){
            boolean success;
            
            //Attempt to make the install directory on the system
            success = skittlePath.mkdirs();
            
            if(!success){
                JOptionPane.showMessageDialog(window, "Error creating installation directory for Skittle.", "Error!", JOptionPane.ERROR_MESSAGE);
                System.exit(2);
            }
        }
        
        //Get the list of all skittle files and their current versions
        downloadFile("files.list");
        window.SetProgressBarPercent(1);
        //Parse the downloaded file list and download all required files
        downloadAllFiles();
    }
    
    /**
     * Function to grab a specified file from the server via http
     * 
     * @param filename the name of the file to pull from the Skittle server
     */
    private void downloadFile(String filename){
        //Setup the URL to begin grabbing files from the server
        String skittleServer = "http://josiah.newlinetechnicalinnovations.com/skittle/";
        URL url;
        URLConnection con;
        int i;
        
        //Get the file list from the server
        try{
            url = new URL(skittleServer + filename);
            con = url.openConnection();
            
            File file = new File(skittlePath + "/" + filename);
            BufferedInputStream bis = new BufferedInputStream(con.getInputStream());
            BufferedOutputStream bos = new BufferedOutputStream(new FileOutputStream(file));
            
            while((i = bis.read()) != -1){
                bos.write(i);
            }
            bos.flush();
            bos.close();
            bis.close();
        }
        catch(MalformedInputException e){
            JOptionPane.showMessageDialog(window, "Malformed packet when downloading file during install. \n" + e.getMessage(), "Error!", JOptionPane.ERROR_MESSAGE);
            System.exit(2);
        }
        catch(IOException e){
            JOptionPane.showMessageDialog(window, "Error writing downloaded file during install. \n" + e.getMessage(), "Error!", JOptionPane.ERROR_MESSAGE);
            System.exit(2);
        }
    }
    
    /**
     * Parse the files.list file list and download each file
     * updating the progress bar as we go.
     * Once the download is done, change the install state of the program to true.
     */
    private void downloadAllFiles(){
        String filesList = skittlePath + "/" + "files.list";
        int numberOfFiles;
        int percentInterval;
        String fileInfo;
        String[] fileInfoSplit;
        
        try{
            //Count the number of files to be downloaded
            LineNumberReader lnr = new LineNumberReader(new FileReader(filesList));
            lnr.skip(Long.MAX_VALUE);
            numberOfFiles = lnr.getLineNumber();
            
            //Calculate percent update per file
            percentInterval = 95 / numberOfFiles;
            window.SetProgressBarPercent(window.GetProgressBarPercent() + 4);
            
            //Start reading the files and downloading them.
            FileInputStream fis = new FileInputStream(filesList);
            
            DataInputStream in = new DataInputStream(fis);
            BufferedReader br = new BufferedReader(new InputStreamReader(in));

            while((fileInfo = br.readLine()) != null){
                //Split the file name from the file version
                fileInfoSplit = fileInfo.split(" ");
                
                //Download the file
                if(isWindows() && !fileInfoSplit[2].contains(".app")){
                    downloadFile(fileInfoSplit[2]);
                }
                else if(isMac() && fileInfoSplit[2].contains(".app")){
                    downloadFile(fileInfoSplit[2]);
                }
                
                //Update the progress bar
                window.SetProgressBarPercent(window.GetProgressBarPercent() + percentInterval);
            }
            
            in.close();
        }
        catch(Exception e){
            JOptionPane.showMessageDialog(window, "Error performing installation. \n" + e.getMessage(), "Error!", JOptionPane.ERROR_MESSAGE);
        }
        
        window.ChangeInstallState(true);
        window.Launch(false);
    }
    
    public static boolean isWindows(){
        return OS.startsWith("Windows");
    }
    
    public static boolean isMac(){
        return OS.startsWith("Mac");
    }
}
