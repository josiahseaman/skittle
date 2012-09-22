package skittle;

import java.awt.Color;

/**
 * Main GUI Window for the Skittle Launcher
 * 
 * This window will launch required actions based on user input.
 * 
 * Actions are:
 * Install, if Skittle isn't installed and the user requests it.
 * Update, if Skittle is installed.
 * Launch, if Skittle is up-to-date and the user requests it.
 * 
 * The window also displays the latest Git commit history.
 * 
 * @author bryan
 */
public class MainWindow extends javax.swing.JFrame {
    /**
     * If the Skittle program is currently installed on the computer
     */
    private boolean installed;

    /**
     * If we should run the update process or not.
     * This is called as an argument when launching the program
     */
    private boolean update;
    
    /**
     * Creates new form MainWindow
     * 
     * @param installed if we have detected if Skittle is installed in the user folder or not
     */
    public MainWindow(boolean installed, boolean update) {
        //Setup all of the graphical components
        initComponents();
        this.installed = installed;
        this.update = update;

        //Hide all buttons and update status items
        actionButton.setVisible(false);
        statusText.setVisible(false);
        statusProgress.setVisible(false);
        progressBar.setVisible(false);
        
        //Set the state of the skittleInstalled text to INSTALLED or NOT INSTALLED
        if(!installed){
            skittleInstalled.setText("NOT INSTALLED");
            skittleInstalled.setForeground(Color.red);
            
            actionButton.setText("Install");
            actionButton.setActionCommand("Install");
            actionButton.setVisible(true);
        }
        else if(update){
            if(checkForUpdates()){
                skittleInstalled.setText("UPDATE AVAILABLE");
                skittleInstalled.setForeground(Color.orange);
                
                doUpdates();
            }
            else{
                skittleInstalled.setText("INSTALLED");
                skittleInstalled.setForeground(Color.green);
            
                statusText.setText("Launching");
                statusText.setVisible(true);
            
                //Launch the skittle program with a flag to identify if updates are available or not
                Launch(false);
            }
        }
        else{
            skittleInstalled.setText("INSTALLED");
            skittleInstalled.setForeground(Color.green);
            
            statusText.setText("Launching");
            statusText.setVisible(true);
            
            //Launch the skittle program with a flag to identify if updates are available or not
            Launch(checkForUpdates());
        }
    }
    
    /**
     * Set the percent complete on the UI progressBar
     * 
     * @param percent the percent done 0-100
     */
    public void SetProgressBarPercent(int percent){
        progressBar.setValue(percent);
    }
    
    /**
     * Change the state of the Skittle program being installed or not
     * and offer correct user actions in the UI depending on state
     * 
     * @param installed if skittle program is installed
     */
    public void ChangeInstallState(boolean installed){
        this.installed = installed;
        
        if(installed){
            skittleInstalled.setText("INSTALLED");
            skittleInstalled.setForeground(Color.green);
            
            actionButton.setText("Launch");
            actionButton.setActionCommand("Launch");
            actionButton.setVisible(true);
            statusText.setVisible(false);
            statusProgress.setVisible(false);
            progressBar.setVisible(false);
        }
    }
    
    /**
     * Update the statusUpdate text "..." to show that work is being done
     * NOTE: NOT Currently Used
     */
    public void ProgressUpdate(){
        if(statusProgress.getText().equalsIgnoreCase(".")){
            statusProgress.setText("..");
        }
        else if(statusProgress.getText().equalsIgnoreCase("..")){
            statusProgress.setText("...");
        }
        else{
            statusProgress.setText(".");
        }
    }
    
    /**
     * Get the percent on the UI progressBar
     * 
     * @return 0-100 percent being displayed on the UI progressBar
     */
    public int GetProgressBarPercent(){
        return progressBar.getValue();
    }
    
    /**
     * This will launch the windows version of Skittle
     * 
     * TODO: Figure out how to launch in linux and mac
     */
    public void Launch(boolean update){
        //Grab the system runtime environment for launching processes
        //Runtime runtime = Runtime.getRuntime();
        //Setup the process for skittle to launch in
        Process process = null;
            
        try{
            //Get the patch to the Windows exe
            String skittlePathExe = System.getProperty("user.home") + "/AppData/Roaming/Skittle/SkittleToo.exe";
                
            //Setup the Skittle program with it's own little process to call home
            //process = runtime.exec(skittlePathExe);
            if(update){
                process = new ProcessBuilder(skittlePathExe, "update").start();
            }
            else{
                process = new ProcessBuilder(skittlePathExe).start();
            }
                
            //Exit the launcher
            System.exit(0);
        }
        catch(Exception e){
            //TODO: Handle an error here
            System.exit(2);
        }
    }
    
    /**
     * Setup a new Update thread and check if there are updates available
     * 
     * @return If there are updates available for Skittle
     */
    private boolean checkForUpdates(){
        //Setup new update thread
        //This must be on a different thread than the event dispatcher so the UI can update
        Update updater = new Update(this);
        new Thread(updater).start();
        
        return updater.CheckForUpdates();
    }
    
    /**
     * Start the install process
     */
    private void install(){
        skittleInstalled.setText("INSTALLING");
        skittleInstalled.setForeground(Color.orange);
        actionButton.setVisible(false);
        statusText.setText("Downloading");
        statusText.setVisible(true);
        statusProgress.setText(":");
        statusProgress.setVisible(true);
        progressBar.setVisible(true);
            
        //Setup a new thread to run the installer.
        //The download process must be on a different thread than the event dispatcher so the UI can update
        new Thread(new Install(this)).start();
    }
    
    /**
     * Start the update process
     */
    private void doUpdates(){
        actionButton.setVisible(false);
        statusText.setText("Updating");
        statusText.setVisible(true);
        statusProgress.setText(":");
        statusProgress.setVisible(true);
        progressBar.setVisible(true);
        
        //Setup a new thread to run the installer.
        //The download process must be on a different thread than the event dispatcher so the UI can update
        Update updater = new Update(this);
        new Thread(updater).start();
        
        updater.DoUpdates();
    }

    /**
     * This method is called from within the constructor to initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is always
     * regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        skittleInstalledStatic = new javax.swing.JLabel();
        skittleInstalled = new javax.swing.JLabel();
        actionButton = new javax.swing.JButton();
        statusText = new javax.swing.JLabel();
        statusProgress = new javax.swing.JLabel();
        progressBar = new javax.swing.JProgressBar();

        setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);
        setTitle("Skittle Launcher");
        setMaximumSize(new java.awt.Dimension(487, 80));
        setName("mainWindow"); // NOI18N
        setPreferredSize(new java.awt.Dimension(487, 80));
        setResizable(false);

        skittleInstalledStatic.setText("Skittle:");

        skittleInstalled.setForeground(new java.awt.Color(255, 0, 0));
        skittleInstalled.setText("UPDATE AVAILABLE");

        actionButton.setText("Install");
        actionButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                actionButtonActionPerformed(evt);
            }
        });

        statusText.setText("Checking for updates");

        statusProgress.setText("...");

        progressBar.setMaximumSize(new java.awt.Dimension(32767, 18));
        progressBar.setMinimumSize(new java.awt.Dimension(10, 18));
        progressBar.setPreferredSize(new java.awt.Dimension(146, 18));

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(skittleInstalledStatic)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(skittleInstalled)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(actionButton)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(statusText)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(statusProgress)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(progressBar, javax.swing.GroupLayout.PREFERRED_SIZE, 174, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap())
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                        .addComponent(skittleInstalledStatic)
                        .addComponent(skittleInstalled)
                        .addComponent(actionButton)
                        .addComponent(statusText)
                        .addComponent(statusProgress))
                    .addComponent(progressBar, javax.swing.GroupLayout.PREFERRED_SIZE, 23, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        pack();
    }// </editor-fold>//GEN-END:initComponents

    /**
     * Install button was pressed by the user, so we should launch the Install process.
     */
    private void actionButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_actionButtonActionPerformed
        if(evt.getActionCommand().equalsIgnoreCase("Install")){
            install();
        }
        else if(evt.getActionCommand().equalsIgnoreCase("Launch")){
            Launch(false);
        }
    }//GEN-LAST:event_actionButtonActionPerformed

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton actionButton;
    private javax.swing.JProgressBar progressBar;
    private javax.swing.JLabel skittleInstalled;
    private javax.swing.JLabel skittleInstalledStatic;
    private javax.swing.JLabel statusProgress;
    private javax.swing.JLabel statusText;
    // End of variables declaration//GEN-END:variables
}
