enum asState {
    AS_OFF,
    AS_READY, 
    AS_DRIVING, 
    AS_FINISHED,
    AS_EMERGENCY,
    AS_MANUAL
};

enum asMission {
    AMI_NONE,
    AMI_ACCELERATION, 
    AMI_SKIDPAD, 
    AMI_TRACKDRIVE, 
    AMI_AUTOCROSS,
    AMI_BRAKETEST,
    AMI_INSPECTION,
    AMI_MANUAL,
    AMI_TEST
};