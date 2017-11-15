//*************************************************
int timed_accept(int sockfd, struct sockaddr *cli_addr, socklen_t *clilen, 
        int timeout)
{
    int result;
    struct timeval tv;
    fd_set rfds;
    printf("FD_SETSIZE: %i\n", FD_SETSIZE);
    printf("sockfd: %i\n", sockfd);

    FD_ZERO(&rfds);
    FD_SET(sockfd, &rfds);

    tv.tv_sec = (long)timeout;
    tv.tv_usec = 0;

    result = select(sockfd+1, &rfds, (fd_set *) 0, (fd_set *) 0, &tv);
    if(result > 0)
    {
        return accept(sockfd, cli_addr, clilen);
    }
    else
    {
        return result;
    }
}

